#pragma once

#include "Graphics/GL/Wrappers/GLFramebuffer.h"
#include "Graphics/GL/Wrappers/GLConstantBuffer.h"
#include "Graphics/GL/Wrappers/GLTextureBuffer.h"
#include "Graphics/GL/Wrappers/GLTexture.h"
#include "Graphics/GL/Wrappers/GLShader.h"
#include "Graphics/GL/Tech/ClusteredShading.h"
#include "Graphics/GL/Tech/HBAO.h"
#include "Graphics/GL/Tech/FXAA.h"
#include "Graphics/GL/Tech/Bloom.h"
#include "Graphics/GL/Tech/CubeMapGen.h"
#include "Graphics/Utils/PerspectiveCamera.h"

#include "EASTL/vector.h"

#include <glm/glm.hpp>

class LightManager;
class GLScene;

class GLRenderer
{
public:

	struct ModelMatrixData
	{
		glm::mat4 u_modelMatrix;
	};

	struct CameraVarsData
	{
		glm::mat4 u_vpMatrix;
		glm::mat4 u_viewMatrix;
		glm::mat4 u_normalMatrix;
		glm::vec3 u_eyePos;
		float u_camNear;
		float u_camFar;
	};

	struct LightingGlobalsData
	{
		glm::vec3 u_ambient;
		float padding_LightningGlobals;
		glm::vec3 u_sunDir;
		float padding2_LightingGlobals;
		glm::vec4 u_sunColorIntensity;
		glm::mat4 u_shadowMat;
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

	void reloadShaders();

	void setSun(const glm::vec3& direction, const glm::vec3& color, float intensity);

	void setHBAOEnabled(bool a_enabled) { m_hbaoEnabled = a_enabled; }
	bool isHBAOEnabled() const { return m_hbaoEnabled; }

	void setFXAAEnabled(bool a_enabled) { m_fxaaEnabled = a_enabled; }
	bool isFXAAEnabled() const { return m_fxaaEnabled; }

	void setBloomEnabled(bool a_enabled) { m_bloomEnabled = a_enabled; }
	bool isBloomEnabled() const { return m_bloomEnabled; }

	void setDepthPrepassEnabled(bool a_enabled);
	bool isDepthPrepassEnabled() const { return m_depthPrepassEnabled; }

	void setShadowsEnabled(bool a_enabled) { m_shadowsEnabled = a_enabled; m_shadowsUpdated = false; }
	bool isShadowsEnabled() const { return m_shadowsEnabled; }

private:

	void updateLightingGlobalsUBO(const PerspectiveCamera& camera);
	void updateCameraDataUBO(const PerspectiveCamera& camera);

private:

	eastl::vector<GLScene*> m_scenes;
	eastl::vector<GLScene*> m_skyboxScenes;

	HBAO m_hbao;
	bool m_hbaoEnabled = true;

	FXAA m_fxaa;
	bool m_fxaaEnabled = true;

	Bloom m_bloom;
	bool m_bloomEnabled = true;

	bool m_depthPrepassEnabled = true;
	ClusteredShading m_clusteredShading;

	bool m_cubeMapGenerated = false;
	CubeMapGen m_cubeMapGenerator;

	GLShader m_depthPrepassShader;
	GLShader m_skyboxShader;
	GLShader m_modelShader;
	GLShader m_combineShader;

	GLFramebuffer m_sceneFBO;
	GLFramebuffer m_shadowFBO;
	PerspectiveCamera m_shadowCamera;

	GLTexture m_whiteTexture;
	GLTexture m_blackTexture;
	GLTexture m_dfvTexture;

	GLConstantBuffer m_modelMatrixUBO;
	GLConstantBuffer m_cameraVarsUBO;
	GLConstantBuffer m_lightningGlobalsUBO;

	glm::vec3 m_sunDir;
	glm::vec4 m_sunColorIntensity;
	bool m_shadowsUpdated = false;

	bool m_shadowsEnabled = true;
};