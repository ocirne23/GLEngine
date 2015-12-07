#pragma once

#include "Graphics/GL/Wrappers/GLFramebuffer.h"
#include "Graphics/GL/Wrappers/GLConstantBuffer.h"
#include "Graphics/GL/Wrappers/GLTextureBuffer.h"
#include "Graphics/GL/Wrappers/GLTexture.h"
#include "Graphics/GL/Wrappers/GLShader.h"
#include "Graphics/Utils/ClusteredShading.h"
#include "Graphics/GL/Tech/HBAO.h"

#include "EASTL/vector.h"

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

	void initialize(const PerspectiveCamera& camera);
	void setHBAOEnabled(bool a_enabled) { m_hbaoEnabled = a_enabled; }
	bool isHBAOEnabled() const { return m_hbaoEnabled; }

	void render(const PerspectiveCamera& camera, const LightManager& lightManager);
	void addScene(GLScene* scene);
	void removeScene(GLScene* scene);

private:

	eastl::vector<GLScene*> m_scenes;

	GLShader m_modelShader;
	HBAO m_hbao;
	bool m_hbaoEnabled = true;

	GLConstantBuffer m_modelMatrixUBO;
	GLConstantBuffer m_cameraVarsUBO;
	GLConstantBuffer m_lightningGlobalsUBO;
	GLConstantBuffer m_lightPositionRangesUBO;
	GLConstantBuffer m_lightColorIntensitiesUBO;
	GLConstantBuffer m_clusteredShadingGlobalsUBO;

	GLTexture m_dfvTexture;
	ClusteredShading m_clusteredShading;
	GLTextureBuffer m_lightIndiceTextureBuffer;
	GLTextureBuffer m_lightGridTextureBuffer;
};