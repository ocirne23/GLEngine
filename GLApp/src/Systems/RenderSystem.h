#pragma once

#include "Core.h"

#include "3rdparty/entityx/System.h"
#include "Graphics/LightManager.h"
#include "Graphics/GL/Wrappers/GLConstantBuffer.h"
#include "Graphics/GL/Wrappers/GLFramebuffer.h"
#include "Graphics/GL/Wrappers/GLShader.h"
#include "Graphics/GL/Wrappers/GLTexture.h"
#include "Graphics/GL/Wrappers/GLTextureBuffer.h"
#include "Graphics/GL/Wrappers/GLUniform.h"
#include "Graphics/GL/Tech/ClusteredShading.h"
#include "Graphics/GL/Tech/HBAO.h"

#include <glm/glm.hpp>

class CameraSystem;
class LightSystem;

BEGIN_NAMESPACE(UBOBindingPoints)
enum
{
	MODEL_MATERIAL_UBO_BINDING_POINT = 0,
	LIGHT_POSITION_RANGE_UBO_BINDING_POINT,
	LIGHT_COLOR_UBO_BINDING_POINT,
	HBAO_GLOBALS_UBO_BINDING_POINT,
	NUM_ENGINE_RESERVED_UBO_BINDING_POINTS
};
END_NAMESPACE(UBOBindingPoints)

BEGIN_NAMESPACE(TextureUnits)
enum
{
	DFV_TEXTURE = 0,
	CLUSTERED_LIGHTING_GRID_TEXTURE,
	CLUSTERED_LIGHTING_LIGHT_ID_TEXTURE,
	MODEL_TEXTURE_ARRAY,
	NUM_ENGINE_RESERVED_TEXTURE_UNITS
};
END_NAMESPACE(TextureUnits)

class RenderSystem : public entityx::System<RenderSystem>
{
public:

	RenderSystem(const CameraSystem& cameraSystem, const LightSystem& lightSystem);
	~RenderSystem();

	void update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt) override;

	const GLShader& getModelShader()    { return m_modelShader; }
	bool isHBAOEnabled() const          { return m_hbaoEnabled; }
	void setHBAOEnabled(bool a_enabled) { m_hbaoEnabled = a_enabled; }

private:

	void initializeShaderForCamera(const PerspectiveCamera& camera);

private:

	const LightSystem& m_lightSystem;
	const CameraSystem& m_cameraSystem;

	bool m_shaderInitialized = false;
	bool m_hbaoEnabled       = true;
	HBAO m_hbao;

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