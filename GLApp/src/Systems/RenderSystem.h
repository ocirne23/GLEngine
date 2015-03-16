#pragma once

#include "Core.h"

#include "entityx/System.h"

#include "Graphics/LightManager.h"
#include "Graphics/GL/Wrappers/GLConstantBuffer.h"
#include "Graphics/GL/Wrappers/GLShader.h"
#include "Graphics/GL/Wrappers/GLTexture.h"
#include "Graphics/GL/Wrappers/GLTextureBuffer.h"
#include "Graphics/GL/Wrappers/GLUniform.h"
#include "Graphics/GL/Tech/ClusteredShading.h"

#include <glm/glm.hpp>

struct CameraComponent;
class PerspectiveCamera;
class LightSystem;

class RenderSystem : public entityx::System<RenderSystem>, public entityx::Receiver<RenderSystem>
{
public:

	enum UBOBindingPoints
	{
		UBOBindingPoints_MODEL_MATERIAL_UBO_BINDING_POINT = 0,
		UBOBindingPoints_LIGHT_POSITION_RANGE_UBO_BINDING_POINT,
		UBOBindingPoints_LIGHT_COLOR_UBO_BINDING_POINT,
		UBOBindingPoints_NUM_ENGINE_RESERVED_UBO_BINDING_POINTS
	};

	enum TextureUnits
	{
		TextureUnits_DFV_TEXTURE = 0,
		TextureUnits_CLUSTERED_LIGHTING_GRID_TEXTURE,
		TextureUnits_CLUSTERED_LIGHTING_LIGHT_ID_TEXTURE,
		TextureUnits_MODEL_TEXTURE_ARRAY,
		TextureUnits_NUM_ENGINE_RESERVED_TEXTURE_UNITS
	};

public:

	RenderSystem(LightSystem& lightSystem);
	~RenderSystem();

	void update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt);
	void receive(const entityx::ComponentAddedEvent<CameraComponent>& cameraComponentAddedEvent);
	void configure(entityx::EventManager& eventManager);

	const GLShader& getModelShader() { return m_modelShader; }

private:

	void initializeShaderForCamera(const PerspectiveCamera& camera);

private:

	LightSystem& m_lightSystem;

	const PerspectiveCamera* m_activeCamera = NULL;

	GLTexture m_dfvTexture;
	GLShader m_modelShader;

	GLConstantBuffer m_lightPositionRangeBuffer;
	GLConstantBuffer m_lightColorBuffer;

	ClusteredShading m_clusteredShading;
	GLTextureBuffer m_lightIndiceTextureBuffer;
	GLTextureBuffer m_lightGridTextureBuffer;

	GLUniform<glm::mat3> m_normalMatrixUniform;
	GLUniform<glm::mat4> m_viewMatrixUniform;
	GLUniform<glm::mat4> m_mvpMatrixUniform;
	GLUniform<glm::mat4> m_transformUniform;

	GLShader m_skyboxShader;
	GLUniform<glm::mat3> m_skyboxNormalMatrixUniform;
	GLUniform<glm::mat4> m_skyboxViewMatrixUniform;
	GLUniform<glm::mat4> m_skyboxMvpMatrixUniform;
	GLUniform<glm::mat4> m_skyboxTransformUniform;
};