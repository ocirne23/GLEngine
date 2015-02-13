#include "RenderSystem.h"

#include "Components/CameraComponent.h"
#include "Components/TransformComponent.h"
#include "Components/ModelComponent.h"

#include "entityx/Entity.h"
#include "GLEngine.h"
#include "Graphics/Graphics.h"
#include "Graphics/PerspectiveCamera.h"
#include "Graphics/GL/GLMesh.h"
#include "Graphics/GL/GLVars.h"
#include "Systems/LightSystem.h"

#include <glm/gtc/matrix_transform.hpp>

BEGIN_UNNAMED_NAMESPACE()

static const unsigned int TILE_WIDTH_PX = 32;
static const unsigned int TILE_HEIGHT_PX = 32;

END_UNNAMED_NAMESPACE()

RenderSystem::RenderSystem(LightSystem& a_lightSystem) : m_lightSystem(a_lightSystem)
{
	m_dfvTexture.initialize("Utils/ggx-helper-dfv.da", GLVars::TextureUnits_DFV_TEXTURE, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	m_dfvTexture.bind();
}

RenderSystem::~RenderSystem()
{
}

void RenderSystem::configure(entityx::EventManager& a_eventManager)
{
	a_eventManager.subscribe<entityx::ComponentAddedEvent<CameraComponent>>(*this);
	a_eventManager.subscribe<entityx::ComponentRemovedEvent<CameraComponent>>(*this);
}

void RenderSystem::initializeShaderForCamera(const PerspectiveCamera& camera)
{
	print("RenderSystem::initializeShaderForCamera\n");
	m_clusteredShading.resize(TILE_WIDTH_PX, TILE_HEIGHT_PX, GLEngine::graphics->getScreenWidth(), GLEngine::graphics->getScreenHeight(), camera);

	rde::vector<rde::string> defines;
	//defines.push_back(rde::string("VISUALIZE_LIGHT_CULLING"));
	defines.push_back(rde::string("MAX_LIGHTS ").append(rde::to_string((int) LightSystem::MAX_LIGHTS)));
	defines.push_back(rde::string("LIGHT_GRID_WIDTH ").append(rde::to_string(m_clusteredShading.getGridWidth())));
	defines.push_back(rde::string("LIGHT_GRID_HEIGHT ").append(rde::to_string(m_clusteredShading.getGridHeight())));
	defines.push_back(rde::string("LIGHT_GRID_DEPTH ").append(rde::to_string(m_clusteredShading.getGridDepth())));
	defines.push_back(rde::string("LIGHT_GRID_TILE_WIDTH ").append(rde::to_string(TILE_WIDTH_PX)));
	defines.push_back(rde::string("LIGHT_GRID_TILE_HEIGHT ").append(rde::to_string(TILE_HEIGHT_PX)));

	m_modelShader.initialize("Shaders/modelshader.vert", "Shaders/modelshader.frag", &defines);

	m_modelShader.begin();
	m_modelShader.setUniform1i("u_dfvTexture", GLVars::TextureUnits_DFV_TEXTURE);
	m_modelShader.setUniform1i("u_textureArray", GLVars::TextureUnits_MODEL_TEXTURE_ARRAY);
	m_modelShader.setUniform1f("u_recLogSD1", m_clusteredShading.getRecLogSD1());
	m_modelShader.setUniform1f("u_recNear", m_clusteredShading.getRecNear());
	m_modelShader.setUniform3f("u_ambient", glm::vec3(0.05f));

	m_viewMatrixUniform.initialize(m_modelShader, "u_mv");
	m_mvpMatrixUniform.initialize(m_modelShader, "u_mvp");
	m_normalMatrixUniform.initialize(m_modelShader, "u_normalMat");
	m_transformUniform.initialize(m_modelShader, "u_transform");

	m_lightPositionRangeBuffer.initialize(m_modelShader, GLVars::UBOBindingPoints_LIGHT_POSITION_RANGE_UBO_BINDING_POINT, "LightPositionRanges", GL_STREAM_DRAW);
	m_lightColorBuffer.initialize(m_modelShader, GLVars::UBOBindingPoints_LIGHT_COLOR_UBO_BINDING_POINT, "LightColors", GL_STREAM_DRAW);

	m_lightGridTextureBuffer.initialize(m_modelShader, "u_lightGrid", GLVars::TextureUnits_CLUSTERED_LIGHTING_GRID_TEXTURE, GL_RG32UI, GL_STREAM_DRAW);
	m_lightIndiceTextureBuffer.initialize(m_modelShader, "u_lightIndices", GLVars::TextureUnits_CLUSTERED_LIGHTING_LIGHT_ID_TEXTURE, GL_R16UI, GL_STREAM_DRAW);
	m_lightGridTextureBuffer.bind();
	m_lightIndiceTextureBuffer.bind();

	m_modelShader.end();
}

void RenderSystem::receive(const entityx::ComponentAddedEvent<CameraComponent>& a_cameraComponentAddedEvent)
{
	assert(!m_activeCamera);
	m_activeCamera = a_cameraComponentAddedEvent.component->camera;
	initializeShaderForCamera(*m_activeCamera);
}

void RenderSystem::receive(const entityx::ComponentRemovedEvent<CameraComponent>& a_cameraComponentRemovedEvent)
{
	assert(m_activeCamera == a_cameraComponentRemovedEvent.component->camera);
	m_activeCamera = NULL;
}

void RenderSystem::update(entityx::EntityManager& a_entities, entityx::EventManager& a_events, entityx::TimeDelta a_dt)
{
	GLEngine::graphics->clear(glm::vec4(0.4f, 0.7f, 1.0f, 1.0f));

	if (m_activeCamera)
	{
		m_modelShader.begin();

		const PerspectiveCamera& camera = *m_activeCamera;
		
		const glm::vec4* viewspaceLightPositionRanges = m_lightSystem.getViewspaceLightPositionRangeList();
		m_clusteredShading.update(camera, m_lightSystem.getNumLights(), viewspaceLightPositionRanges);

		m_lightPositionRangeBuffer.upload(m_lightSystem.getNumLights() * sizeof(glm::vec4), viewspaceLightPositionRanges);
		m_lightColorBuffer.upload(m_lightSystem.getNumLights() * sizeof(glm::vec4), m_lightSystem.getLightColorList());
		m_lightGridTextureBuffer.upload(m_clusteredShading.getGridSize() * sizeof(glm::uvec2), m_clusteredShading.getLightGrid());
		m_lightIndiceTextureBuffer.upload(m_clusteredShading.getNumLightIndices() * sizeof(ushort), m_clusteredShading.getLightIndices());

		m_mvpMatrixUniform.set(camera.m_combinedMatrix);
		m_viewMatrixUniform.set(camera.m_viewMatrix);
		m_normalMatrixUniform.set(glm::mat3(glm::inverse(glm::transpose(camera.m_viewMatrix))));

		entityx::ComponentHandle<TransformComponent> transform;
		entityx::ComponentHandle<ModelComponent> model;
		for (entityx::Entity entity : a_entities.entities_with_components(transform, model))
		{
			m_transformUniform.set(transform->transform);
			model->mesh->render(m_modelShader);
		}

		m_modelShader.end();
	}

	GLEngine::graphics->swap();
}
