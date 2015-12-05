#include "Graphics/GL/Scene/GLRenderer.h"

#include "Graphics/GL/Scene/GLScene.h"
#include "Graphics/GL/Scene/GLConfig.h"
#include "Graphics/Utils/PerspectiveCamera.h"
#include "Graphics/Utils/LightManager.h"

void GLRenderer::initialize()
{
	m_modelShader.initialize("Shaders/modelshader.vert", "Shaders/modelshadersimple.frag");

	m_modelMatrixUBO.initialize(GLConfig::MODEL_MATRIX_UBO);
	m_cameraVarsUBO.initialize(GLConfig::CAMERA_VARS_UBO);
	m_lightningGlobalsUBO.initialize(GLConfig::LIGHTING_GLOBALS_UBO);
	m_lightPositionRangesUBO.initialize(GLConfig::LIGHT_POSITION_RANGES_UBO);
	m_lightColorIntensitiesUBO.initialize(GLConfig::LIGHT_COLOR_INTENSITIES_UBO);
	m_clusteredShadingGlobalsUBO.initialize(GLConfig::CLUSTERED_SHADING_GLOBALS_UBO);
}

void GLRenderer::renderScene(GLScene& a_scene, const PerspectiveCamera& a_camera)
{
	m_modelShader.begin();

	// Update camera data
	CameraVarsUBO* cameraVars = (CameraVarsUBO*) m_cameraVarsUBO.mapBuffer();
	cameraVars->u_vpMatrix     = a_camera.getCombinedMatrix();
	cameraVars->u_viewMatrix   = a_camera.getViewMatrix();
	cameraVars->u_normalMatrix = glm::inverse(glm::transpose(a_camera.getViewMatrix()));
	cameraVars->u_eyePos       = a_camera.getPosition();
	m_cameraVarsUBO.unmapBuffer();

	a_scene.render(m_modelMatrixUBO);

	m_modelShader.end();
}
