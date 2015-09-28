#include "RenderSystem.h"

#include "Components/CameraComponent.h"
#include "Components/TransformComponent.h"
#include "Components/ModelComponent.h"
#include "Components/SkyComponent.h"
#include "Components/UIComponent.h"
#include "GLEngine.h"
#include "Graphics/Graphics.h"
#include "Graphics/PerspectiveCamera.h"
#include "Graphics/GL/GLMesh.h"
#include "Systems/LightSystem.h"
#include "Systems/CameraSystem.h"
#include "Systems/UISystem.h"
#include "UI/UIFrame.h"
#include "Utils/FileModificationManager.h"
#include "3rdparty/entityx/Entity.h"

#include <glm/gtc/matrix_transform.hpp>

BEGIN_UNNAMED_NAMESPACE()

static const char* const DFV_TEX_PATH = "Utils/ggx-helper-dfv.hdr.da";
static const char* const CLUSTERED_SHADING_PATH = "Shaders/clusteredshading.glsl";
static const char* const MATERIAL_LIGHTING_PATH = "Shaders/material.glsl";
static const char* const CAMERA_VARS_PATH = "Shaders/cameravars.glsl";
static const char* const MODEL_VERT_SHADER_PATH = "Shaders/modelshader.vert";
static const char* const MODEL_FRAG_SHADER_PATH = "Shaders/modelshader.frag";
static const char* const SKYBOX_FRAG_SHADER_PATH = "Shaders/skyboxshader.frag";

static const uint UI_SPRITE_BATCH_SIZE = 100;
static const uint TILE_WIDTH_PX  = 64;
static const uint TILE_HEIGHT_PX = 64;

static const glm::vec3 AMBIENT(0.15f);

struct CameraVars
{
	glm::mat4 u_vpMatrix;
	glm::mat4 u_viewMatrix;
	glm::mat4 u_normalMatrix;
};

END_UNNAMED_NAMESPACE()

RenderSystem::RenderSystem(const CameraSystem& a_cameraSystem, const LightSystem& a_lightSystem, const UISystem& a_uiSystem) :
m_cameraSystem(a_cameraSystem), m_lightSystem(a_lightSystem), m_uiSystem(a_uiSystem)
{
	m_fpsMeasurer.setLogFunction(5.0f, [](const FPSMeasurer& a_measurer)
	{
		print("FPS: %i \t MS: %f\n", a_measurer.getAvgFps(), a_measurer.getAvgMsPerFrame());
	});

	m_xRes = GLEngine::graphics->getViewportWidth();
	m_yRes = GLEngine::graphics->getViewportHeight();

	m_dfvTexture.initialize(DFV_TEX_PATH, GLTexture::ETextureMinFilter::LINEAR, GLTexture::ETextureMagFilter::LINEAR, GLTexture::ETextureWrap::CLAMP_TO_EDGE, GLTexture::ETextureWrap::CLAMP_TO_EDGE);
	m_uiSpriteBatch.initialize(UI_SPRITE_BATCH_SIZE);

	auto onShaderEdited = [&]()
	{
		print("shader edited\n");
		const PerspectiveCamera* camera = m_cameraSystem.getActiveCamera();
		if (camera)
			initializeShaderForCamera(*camera);
	};
	FileModificationManager::createModificationListener(this, rde::string(MODEL_VERT_SHADER_PATH), onShaderEdited);
	FileModificationManager::createModificationListener(this, rde::string(MODEL_FRAG_SHADER_PATH), onShaderEdited);
	FileModificationManager::createModificationListener(this, rde::string(CLUSTERED_SHADING_PATH), onShaderEdited);
	FileModificationManager::createModificationListener(this, rde::string(MATERIAL_LIGHTING_PATH), onShaderEdited);

	onResize(m_xRes, m_yRes);
}

RenderSystem::~RenderSystem()
{
	FileModificationManager::removeAllModificationListenersForOwner(this);
}

void RenderSystem::onResize(uint a_width, uint a_height)
{
	m_xRes = a_width;
	m_yRes = a_height;

	glm::mat4 projectionMatrix = glm::ortho(0.0f, (float) m_xRes, (float) m_yRes, 0.0f, 0.1f, 100.0f);
	glm::mat4 viewMatrix = glm::lookAt(glm::vec3(0, 0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	m_uiMatrix = projectionMatrix * viewMatrix;

	const PerspectiveCamera* camera = m_cameraSystem.getActiveCamera();
	if (camera)
		initializeShaderForCamera(*camera);
}

void RenderSystem::initializeShaderForCamera(const PerspectiveCamera& a_camera)
{
	m_clusteredShading.initialize(a_camera, m_xRes, m_yRes, TILE_WIDTH_PX, TILE_HEIGHT_PX);
	m_hbao.initialize(a_camera, m_xRes, m_yRes, UBOBindingPoints::HBAO_GLOBALS_UBO_BINDING_POINT);

	rde::vector<rde::string> defines;
	defines.push_back(rde::string("MAX_LIGHTS ").append(rde::to_string((int) LightSystem::MAX_LIGHTS)));
	defines.push_back(rde::string("LIGHT_GRID_WIDTH ").append(rde::to_string(m_clusteredShading.getGridWidth())));
	defines.push_back(rde::string("LIGHT_GRID_HEIGHT ").append(rde::to_string(m_clusteredShading.getGridHeight())));
	defines.push_back(rde::string("LIGHT_GRID_DEPTH ").append(rde::to_string(m_clusteredShading.getGridDepth())));
	defines.push_back(rde::string("LIGHT_GRID_TILE_WIDTH ").append(rde::to_string(TILE_WIDTH_PX)));
	defines.push_back(rde::string("LIGHT_GRID_TILE_HEIGHT ").append(rde::to_string(TILE_HEIGHT_PX)));

	m_modelShader.initialize(MODEL_VERT_SHADER_PATH, MODEL_FRAG_SHADER_PATH, &defines);
	m_modelShader.begin();
	{
		m_modelShader.setUniform1i("u_dfvTexture", TextureUnits::DFV_TEXTURE);
		m_modelShader.setUniform1i("u_textureArray", TextureUnits::MODEL_TEXTURE_ARRAY);
		m_modelShader.setUniform1i("u_lightGrid", TextureUnits::CLUSTERED_LIGHTING_GRID_TEXTURE);
		m_modelShader.setUniform1i("u_lightIndices", TextureUnits::CLUSTERED_LIGHTING_LIGHT_ID_TEXTURE);

		m_modelShader.setUniform1f("u_recLogSD1", m_clusteredShading.getRecLogSD1());
		m_modelShader.setUniform1f("u_recNear", m_clusteredShading.getRecNear());
		m_modelShader.setUniform3f("u_ambient", AMBIENT);

		m_modelMatrixUniform.initialize(m_modelShader, "u_modelMatrix");

		m_lightPositionRangeBuffer.initialize(m_modelShader, UBOBindingPoints::LIGHT_POSITION_RANGE_UBO_BINDING_POINT, "LightPositionRanges", GLConstantBuffer::EDrawUsage::STREAM);
		m_lightColorBuffer.initialize(m_modelShader, UBOBindingPoints::LIGHT_COLOR_UBO_BINDING_POINT, "LightColorsIntensities", GLConstantBuffer::EDrawUsage::STREAM);
	}
	m_modelShader.end();

	m_lightGridTextureBuffer.initialize(GLTextureBuffer::ESizedFormat::RG32I, GLTextureBuffer::EDrawUsage::STREAM);
	m_lightIndiceTextureBuffer.initialize(GLTextureBuffer::ESizedFormat::R16I, GLTextureBuffer::EDrawUsage::STREAM);

	m_skyboxShader.initialize(MODEL_VERT_SHADER_PATH, SKYBOX_FRAG_SHADER_PATH);
	m_skyboxShader.begin();
	{
		m_skyboxShader.setUniform1i("u_textureArray", TextureUnits::MODEL_TEXTURE_ARRAY);
		m_skyboxModelMatrixUniform.initialize(m_skyboxShader, "u_modelMatrix");
	}
	m_skyboxShader.end();

	const GLShader* shaders[]{&m_modelShader, &m_skyboxShader};
	m_cameraBuffer.initialize(shaders, ARRAY_SIZE(shaders), UBOBindingPoints::CAMERA_VARS_UBO_BINDING_POINT, "CameraVars", GLConstantBuffer::EDrawUsage::STREAM);

	m_shaderInitialized = true;
}

void RenderSystem::update(entityx::EntityManager& a_entities, entityx::EventManager& a_events, entityx::TimeDelta a_dt)
{
	const PerspectiveCamera* activeCamera = m_cameraSystem.getActiveCamera();

	if (!m_shaderInitialized && activeCamera)
		initializeShaderForCamera(*activeCamera);

	if (!activeCamera)
	{
		GLEngine::graphics->clear(glm::vec4(0, 1, 0, 1));
		GLEngine::graphics->swap();
		return;
	}

	CameraVars cameraVars;
	cameraVars.u_vpMatrix = activeCamera->getCombinedMatrix();
	cameraVars.u_viewMatrix = activeCamera->getViewMatrix();
	cameraVars.u_normalMatrix = glm::inverse(glm::transpose(activeCamera->getViewMatrix()));
	m_cameraBuffer.upload(sizeof(CameraVars), &cameraVars);
	m_cameraBuffer.bind();

	if (m_hbaoEnabled)
		m_hbao.begin();

	renderSkybox(a_entities);

	// No need to clear color because skybox fully covers previous frame
	GLEngine::graphics->clear(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), false, true);
	
	renderModels(a_entities);
	
	if (m_hbaoEnabled)
		m_hbao.endAndRender();
	
	renderUI();

	GLEngine::graphics->swap();

	m_fpsMeasurer.tickFrame(a_dt);
}

void RenderSystem::renderSkybox(entityx::EntityManager& a_entities)
{
	// Skybox doesn't need to look at or modify depth buffer
	GLEngine::graphics->setDepthWrite(false);
	GLEngine::graphics->setDepthTest(false);

	// Might need to render skybox last with depth test, uses less bandwidth.
	m_skyboxShader.begin();
	{
		entityx::ComponentHandle<SkyComponent> sky;
		entityx::ComponentHandle<ModelComponent> model;
		for (entityx::Entity entity : a_entities.entities_with_components(sky, model))
		{
			glm::mat4 trans = sky->transform;
			glm::vec3 pos = m_cameraSystem.getActiveCamera()->getPosition();

			if (sky->centerOnCamera)
				trans[3] = glm::vec4(pos, 1.0f);

			m_skyboxModelMatrixUniform.set(trans);
			model->mesh->render(m_skyboxShader);
		}
	}
	m_skyboxShader.end();

	GLEngine::graphics->setDepthWrite(true);
	GLEngine::graphics->setDepthTest(true);
}

void RenderSystem::renderModels(entityx::EntityManager& a_entities)
{
	const glm::vec4* viewspaceLightPositionRanges = m_lightSystem.getViewspaceLightPositionRangeList();
	m_clusteredShading.update(*m_cameraSystem.getActiveCamera(), m_lightSystem.getNumLights(), viewspaceLightPositionRanges);

	m_dfvTexture.bind(TextureUnits::DFV_TEXTURE);
	m_lightGridTextureBuffer.bind(TextureUnits::CLUSTERED_LIGHTING_GRID_TEXTURE);
	m_lightIndiceTextureBuffer.bind(TextureUnits::CLUSTERED_LIGHTING_LIGHT_ID_TEXTURE);

	m_modelShader.begin();
	{
		m_lightPositionRangeBuffer.upload(m_lightSystem.getNumLights() * sizeof(glm::vec4), viewspaceLightPositionRanges);
		m_lightColorBuffer.upload(m_lightSystem.getNumLights() * sizeof(glm::vec4), m_lightSystem.getLightColorIntensityList());

		m_lightGridTextureBuffer.upload(m_clusteredShading.getGridSize() * sizeof(glm::uvec2), m_clusteredShading.getLightGrid());
		m_lightIndiceTextureBuffer.upload(m_clusteredShading.getNumLightIndices() * sizeof(ushort), m_clusteredShading.getLightIndices());

		entityx::ComponentHandle<TransformComponent> transform;
		entityx::ComponentHandle<ModelComponent> model;
		for (entityx::Entity entity : a_entities.entities_with_components(transform, model))
		{
			m_modelMatrixUniform.set(transform->transform);
			model->mesh->render(m_modelShader);
		}
	}
	m_modelShader.end();
}

void RenderSystem::renderUI()
{
	GLEngine::graphics->setBlending(true);
	GLEngine::graphics->setDepthTest(false);

	m_uiSpriteBatch.begin(m_uiMatrix);
	for (const UIComponent* uiComponent : m_uiSystem.getSortedUIComponents())
		uiComponent->getUIFrame().render(m_uiSpriteBatch);
	m_uiSpriteBatch.end();

	GLEngine::graphics->setDepthTest(true);
	GLEngine::graphics->setBlending(false);
}
