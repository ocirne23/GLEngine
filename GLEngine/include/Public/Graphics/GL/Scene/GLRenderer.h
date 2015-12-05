#pragma once

#include "Graphics/GL/Wrappers/GLConstantBuffer.h"
#include "Graphics/GL/Wrappers/GLShader.h"

#include <glm/glm.hpp>

class PerspectiveCamera;
class LightManager;
class GLScene;

class GLRenderer
{
public:

	struct ModelMatrixUBO
	{
		glm::mat4 u_modelMatrix;
	};

	struct CameraVarsUBO
	{
		glm::mat4 u_vpMatrix;
		glm::mat4 u_viewMatrix;
		glm::mat4 u_normalMatrix;
		glm::vec3 u_eyePos;
		float padding_CameraVars;
	};

	struct LightingGlobalsUBO
	{
		glm::vec3 u_ambient;
		float padding_LightningGlobals;
	};

public:

	GLRenderer() {}
	GLRenderer(const GLRenderer& copy) = delete;

	void initialize();
	void renderScene(GLScene& scene, const PerspectiveCamera& camera);

private:

	GLShader m_modelShader;

	GLConstantBuffer m_modelMatrixUBO;
	GLConstantBuffer m_cameraVarsUBO;
	GLConstantBuffer m_lightningGlobalsUBO;
	GLConstantBuffer m_lightPositionRangesUBO;
	GLConstantBuffer m_lightColorIntensitiesUBO;
	GLConstantBuffer m_clusteredShadingGlobalsUBO;
};