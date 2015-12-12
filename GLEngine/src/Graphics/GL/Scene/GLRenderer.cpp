#include "Graphics/GL/Scene/GLRenderer.h"

#include "Database/Assets/DBTexture.h"
#include "Graphics/GL/Scene/GLScene.h"
#include "Graphics/GL/Scene/GLConfig.h"
#include "Graphics/Utils/PerspectiveCamera.h"
#include "Graphics/Utils/LightManager.h"
#include "Graphics/GL/Tech/QuadDrawer.h"
#include "GLEngine.h"
#include "Graphics/Graphics.h"
#include "Utils/StringUtils.h"

BEGIN_UNNAMED_NAMESPACE()

static const char* const DFV_TEX_PATH = "Assets/Utils/ggx-helper-dfv.hdr";
static const char* const WHITE_TEX_PATH = "Assets/Utils/white.png";
static const char* const BLACK_TEX_PATH = "Assets/Utils/black.png";

static const char* const MODEL_VERT_SHADER_PATH = "Shaders/modelshader.vert";
static const char* const MODEL_FRAG_SHADER_PATH = "Shaders/modelshader.frag";
static const char* const SKYBOX_FRAG_SHADER_PATH = "Shaders/skyboxshader.frag";

static const char* const DEPTH_VERT_SHADER_PATH = "Shaders/depth.vert";
static const char* const DEPTH_FRAG_SHADER_PATH = "Shaders/depth.frag";

static const char* const COMBINE_VERT_SHADER_PATH = "Shaders/quad.vert";
static const char* const COMBINE_FRAG_SHADER_PATH = "Shaders/combine.frag";

static const char* const FXAA_VERT_SHADER_PATH = "Shaders/FXAA/FXAA.vert";
static const char* const FXAA_FRAG_SHADER_PATH = "Shaders/FXAA/FXAA_High_Quality.frag";

static const glm::vec3 AMBIENT(0.15f);

END_UNNAMED_NAMESPACE()

void GLRenderer::initialize(const PerspectiveCamera& a_camera)
{
	uint screenWidth = GLEngine::graphics->getViewportWidth();
	uint screenHeight = GLEngine::graphics->getViewportHeight();

	m_sceneFBO.initialize();
	m_sceneFBO.addFramebufferTexture(GLFramebuffer::ESizedFormat::RGB8, GLFramebuffer::EAttachment::COLOR0, screenWidth, screenHeight);
	m_sceneFBO.setDepthbufferTexture(GLFramebuffer::ESizedFormat::DEPTH24, screenWidth, screenHeight);

	m_clusteredShading.initialize(a_camera, screenWidth, screenHeight);
	m_hbao.initialize(a_camera, screenWidth, screenHeight);
	m_bloom.initialize(screenWidth, screenHeight);
	m_fxaa.initialize(FXAA::EQuality::EXTREME, screenWidth, screenHeight);

	m_modelMatrixUBO.initialize(GLConfig::MODEL_MATRIX_UBO);
	m_cameraVarsUBO.initialize(GLConfig::CAMERA_VARS_UBO);
	m_lightningGlobalsUBO.initialize(GLConfig::LIGHTING_GLOBALS_UBO);

	m_depthPrepassShader.initialize(DEPTH_VERT_SHADER_PATH, DEPTH_FRAG_SHADER_PATH, &GLConfig::getGlobalShaderDefines());
	m_modelShader.initialize(MODEL_VERT_SHADER_PATH, MODEL_FRAG_SHADER_PATH, &GLConfig::getGlobalShaderDefines());
	m_skyboxShader.initialize(MODEL_VERT_SHADER_PATH, SKYBOX_FRAG_SHADER_PATH, &GLConfig::getGlobalShaderDefines());
	m_combineShader.initialize(COMBINE_VERT_SHADER_PATH, COMBINE_FRAG_SHADER_PATH, &GLConfig::getGlobalShaderDefines());

	m_whiteTexture.initialize(DBTexture(WHITE_TEX_PATH));
	m_blackTexture.initialize(DBTexture(BLACK_TEX_PATH));
	m_dfvTexture.initialize(DBTexture(DFV_TEX_PATH, DBTexture::EFormat::FLOAT), 0, GLTexture::ETextureMinFilter::LINEAR, GLTexture::ETextureMagFilter::LINEAR);

	LightingGlobalsUBO* lightingGlobals = (LightingGlobalsUBO*) m_lightningGlobalsUBO.mapBuffer();
	lightingGlobals->u_ambient = AMBIENT;
	m_lightningGlobalsUBO.unmapBuffer();
}

void GLRenderer::render(const PerspectiveCamera& a_camera, const LightManager& a_lightManager)
{
	GLEngine::graphics->clear(glm::vec4(0.8f, 0.8f, 0.8f, 1.0f));

	// UPDATE CAMERA DATA //
	CameraVarsUBO* cameraVars  = (CameraVarsUBO*) m_cameraVarsUBO.mapBuffer();
	cameraVars->u_vpMatrix     = a_camera.getCombinedMatrix();
	cameraVars->u_viewMatrix   = a_camera.getViewMatrix();
	cameraVars->u_normalMatrix = glm::inverse(glm::transpose(a_camera.getViewMatrix()));
	cameraVars->u_eyePos       = glm::vec3(a_camera.getViewMatrix() * glm::vec4(a_camera.getPosition(), 1.0));
	m_cameraVarsUBO.unmapBuffer();

	m_clusteredShading.update(a_camera, a_lightManager);
	m_clusteredShading.bindTextureBuffers();
	m_dfvTexture.bind(GLConfig::DFV_TEXTURE_BINDING_POINT);

	m_sceneFBO.begin();
	GLEngine::graphics->clear(glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));
	GLEngine::graphics->setDepthTest(true);

	// DEPTH PREPASS //
	if (m_depthPrepassEnabled)
	{
		GLEngine::graphics->beginDepthPrepass();
		GLEngine::graphics->clearDepthOnly();
		m_depthPrepassShader.begin();
		for (GLScene* scene : m_scenes)
			scene->render(m_modelMatrixUBO);
		m_depthPrepassShader.end();
		GLEngine::graphics->endDepthPrepass();
	}

	// RENDER SKYBOXES //
	m_skyboxShader.begin();
	for (GLScene* scene : m_skyboxScenes)
	{
		scene->setTranslation(a_camera.getPosition());
		scene->render(m_modelMatrixUBO);
	}
	m_skyboxShader.end();
	
	// RENDER MODELS //
	m_modelShader.begin();
	for (GLScene* scene : m_scenes)
		scene->render(m_modelMatrixUBO);
	m_modelShader.end();
	
	m_sceneFBO.end();

	GLEngine::graphics->setDepthTest(false);

	// APPLY HBAO //
	GLFramebuffer* hbaoResult = NULL;
	if (m_hbaoEnabled)
		hbaoResult = &m_hbao.getHBAOResultFBO(m_sceneFBO);

	// APPLY BLOOM // 
	GLFramebuffer* bloomResult = NULL;
	if (m_bloomEnabled)
		bloomResult = &m_bloom.getBloomResultFBO(m_sceneFBO);

	// BIND COMBINED TEXTURES // 
	m_sceneFBO.bindTexture(0, GLConfig::COLOR_TEXTURE_BINDING_POINT);
	if (m_hbaoEnabled)
		hbaoResult->bindTexture(0, GLConfig::AO_RESULT_TEXTURE_BINDING_POINT);
	else
		m_whiteTexture.bind(GLConfig::AO_RESULT_TEXTURE_BINDING_POINT);

	if (m_bloomEnabled)
		bloomResult->bindTexture(0, GLConfig::BLOOM_RESULT_TEXTURE_BINDING_POINT);
	else
		m_blackTexture.bind(GLConfig::BLOOM_RESULT_TEXTURE_BINDING_POINT);

	// APPLY FXAA AND RENDER COMBINED SCENE //
	if (m_fxaaEnabled)
		m_fxaa.begin();
	m_combineShader.begin();
	QuadDrawer::drawQuad(m_combineShader);
	m_combineShader.end();
	if (m_fxaaEnabled)
		m_fxaa.endAndRender();

	GLEngine::graphics->setDepthTest(true);
}

void GLRenderer::addScene(GLScene* a_scene)
{
	m_scenes.push_back(a_scene);
}

void GLRenderer::removeScene(GLScene* a_scene)
{
	auto it = eastl::find(m_scenes.begin(), m_scenes.end(), a_scene);
	if (it != m_scenes.end())
		m_scenes.erase(it);
}

void GLRenderer::addSkybox(GLScene* a_scene)
{
	m_skyboxScenes.push_back(a_scene);
}

void GLRenderer::removeSkybox(GLScene* a_scene)
{
	auto it = eastl::find(m_skyboxScenes.begin(), m_skyboxScenes.end(), a_scene);
	if (it != m_scenes.end())
		m_skyboxScenes.erase(it);
}

void GLRenderer::setDepthPrepassEnabled(bool a_enabled)
{
	if (m_depthPrepassEnabled && !a_enabled)
		GLEngine::graphics->disableDepthPrepass();
	m_depthPrepassEnabled = a_enabled;
}

void GLRenderer::setMaterial(float a_smoothness, bool a_isMetal)
{
	//print("smoothness: %f, isMetal: %i\n", a_smoothness, a_isMetal);
	LightingGlobalsUBO* lightingGlobals = (LightingGlobalsUBO*) m_lightningGlobalsUBO.mapBuffer();
	lightingGlobals->u_ambient = AMBIENT;
	lightingGlobals->u_smoothness = a_smoothness;
	lightingGlobals->u_metalness = a_isMetal ? 1.0f : 0.0f;
	m_lightningGlobalsUBO.unmapBuffer();
}
