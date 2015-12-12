#pragma once

#include "Graphics/GL/Wrappers/GLFramebuffer.h"
#include "Graphics/GL/Wrappers/GLConstantBuffer.h"
#include "Graphics/GL/Wrappers/GLTextureBuffer.h"
#include "Graphics/GL/Wrappers/GLTexture.h"
#include "Graphics/GL/Wrappers/GLShader.h"
#include "Graphics/Utils/ClusteredShading.h"
#include "Graphics/GL/Tech/HBAO.h"
#include "Graphics/GL/Tech/FXAA.h"
#include "Graphics/GL/Tech/Bloom.h"

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
	void render(const PerspectiveCamera& camera, const LightManager& lightManager);
	void addScene(GLScene* scene);
	void removeScene(GLScene* scene);
	void addSkybox(GLScene* scene);
	void removeSkybox(GLScene* scene);

	void setHBAOEnabled(bool a_enabled) { m_hbaoEnabled = a_enabled; }
	bool isHBAOEnabled() const { return m_hbaoEnabled; }

	void setFXAAEnabled(bool a_enabled) { m_fxaaEnabled = a_enabled; }
	bool isFXAAEnabled() const { return m_fxaaEnabled; }

	void setBloomEnabled(bool a_enabled) { m_bloomEnabled = a_enabled; }
	bool isBloomEnabled() const { return m_bloomEnabled; }

private:

	eastl::vector<GLScene*> m_scenes;
	eastl::vector<GLScene*> m_skyboxScenes;

	HBAO m_hbao;
	bool m_hbaoEnabled = true;

	FXAA m_fxaa;
	bool m_fxaaEnabled = true;

	Bloom m_bloom;
	bool m_bloomEnabled = true;

	ClusteredShading m_clusteredShading;

	GLShader m_skyboxShader;
	GLShader m_modelShader;
	GLShader m_combineShader;

	GLFramebuffer m_sceneFBO;

	GLTexture m_whiteTexture;
	GLTexture m_blackTexture;

	GLTexture m_dfvTexture;
	GLTextureBuffer m_lightIndiceTextureBuffer;
	GLTextureBuffer m_lightGridTextureBuffer;

	GLConstantBuffer m_modelMatrixUBO;
	GLConstantBuffer m_cameraVarsUBO;
	GLConstantBuffer m_lightningGlobalsUBO;
	GLConstantBuffer m_lightPositionRangesUBO;
	GLConstantBuffer m_lightColorIntensitiesUBO;
	GLConstantBuffer m_clusteredShadingGlobalsUBO;
};