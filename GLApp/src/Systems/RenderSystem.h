#pragma once

#include "Core.h"

#include "AppUtils/FPSMeasurer.h"
#include "Graphics/Utils/LightManager.h"
#include "Graphics/GL/Tech/SpriteBatch.h"
#include "Graphics/GL/Wrappers/GLConstantBuffer.h"
#include "Graphics/GL/Wrappers/GLFramebuffer.h"
#include "Graphics/GL/Wrappers/GLShader.h"
#include "Graphics/GL/Wrappers/GLTexture.h"
#include "Graphics/GL/Wrappers/GLTextureBuffer.h"
#include "Graphics/GL/Wrappers/GLUniform.h"
#include "Graphics/Utils/ClusteredShading.h"
#include "Graphics/GL/Tech/HBAO.h"
#include "entityx/System.h"

#include <glm/glm.hpp>

class CameraSystem;
class LightSystem;
class UISystem;

BEGIN_NAMESPACE(UBOBindingPoints)
enum
{
	MODEL_MATERIAL_UBO_BINDING_POINT = 0,
	LIGHT_POSITION_RANGE_UBO_BINDING_POINT,
	LIGHT_COLOR_UBO_BINDING_POINT,
	HBAO_GLOBALS_UBO_BINDING_POINT,
	CAMERA_VARS_UBO_BINDING_POINT,
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

	RenderSystem(const CameraSystem& cameraSystem, const LightSystem& lightSystem, const UISystem& uiSystem);
	~RenderSystem();

	void update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt) override;
	void onResize(uint width, uint height);

	const GLShader& getModelShader()    { return m_modelShader; }
	bool isHBAOEnabled() const          { return m_hbaoEnabled; }
	void setHBAOEnabled(bool a_enabled) { m_hbaoEnabled = a_enabled; }

private:

	void initializeShaderForCamera(const PerspectiveCamera& camera);
	void renderSkybox(entityx::EntityManager& entities);
	void renderModels(entityx::EntityManager& entities);
	void renderUI();

private:

	const LightSystem& m_lightSystem;
	const CameraSystem& m_cameraSystem;
	const UISystem& m_uiSystem;

	bool m_shaderInitialized = false;
	uint m_xRes = 0;
	uint m_yRes = 0;

	bool m_hbaoEnabled = true;
	HBAO m_hbao;

	GLTexture m_dfvTexture;

	GLConstantBuffer m_cameraBuffer;

	ClusteredShading m_clusteredShading;
	GLTextureBuffer m_lightIndiceTextureBuffer;
	GLTextureBuffer m_lightGridTextureBuffer;
	GLConstantBuffer m_lightPositionRangeBuffer;
	GLConstantBuffer m_lightColorBuffer;

	GLShader m_modelShader;
	GLUniform<glm::mat4> m_modelMatrixUniform;

	GLShader m_skyboxShader;
	GLUniform<glm::mat4> m_skyboxModelMatrixUniform;

	glm::mat4 m_uiMatrix;
	SpriteBatch m_uiSpriteBatch;

	FPSMeasurer m_fpsMeasurer;
};