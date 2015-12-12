#include "Graphics/GL/Scene/GLRenderer.h"

#include "Database/Assets/DBTexture.h"
#include "Graphics/GL/Scene/GLScene.h"
#include "Graphics/GL/Scene/GLConfig.h"
#include "Graphics/Utils/PerspectiveCamera.h"
#include "Graphics/Utils/LightManager.h"
#include "Graphics/GL/Tech/QuadDrawer.h"
#include "GLEngine.h"
#include "Graphics/Graphics.h"
#include "EASTL/to_string.h"

BEGIN_UNNAMED_NAMESPACE()

static const char* const DFV_TEX_PATH = "Assets/Utils/ggx-helper-dfv.hdr";
static const char* const WHITE_TEX_PATH = "Assets/Utils/white.png";
static const char* const BLACK_TEX_PATH = "Assets/Utils/black.png";

static const char* const MODEL_VERT_SHADER_PATH = "Shaders/modelshader.vert";
static const char* const MODEL_FRAG_SHADER_PATH = "Shaders/modelshader.frag";
static const char* const SKYBOX_FRAG_SHADER_PATH = "Shaders/skyboxshader.frag";


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

	m_sceneFBO.initialize(GLConfig::MULTISAMPLE_TYPE);
	m_sceneFBO.addFramebufferTexture(GLFramebuffer::ESizedFormat::RGB8, GLFramebuffer::EAttachment::COLOR0, screenWidth, screenHeight);
	m_sceneFBO.setDepthbufferTexture(GLFramebuffer::ESizedFormat::DEPTH24, screenWidth, screenHeight);

	m_fxaa.initialize(FXAA::EQuality::EXTREME, screenWidth, screenHeight);
	m_hbao.initialize(a_camera, screenWidth, screenHeight);
	m_bloom.initialize(screenWidth, screenHeight);
	m_clusteredShading.initialize(a_camera, screenWidth, screenHeight, GLConfig::CLUSTERED_SHADING_TILE_WIDTH, GLConfig::CLUSTERED_SHADING_TILE_HEIGHT);

	m_modelMatrixUBO.initialize(GLConfig::MODEL_MATRIX_UBO);
	m_cameraVarsUBO.initialize(GLConfig::CAMERA_VARS_UBO);
	m_lightningGlobalsUBO.initialize(GLConfig::LIGHTING_GLOBALS_UBO);
	m_lightPositionRangesUBO.initialize(GLConfig::LIGHT_POSITION_RANGES_UBO);
	m_lightColorIntensitiesUBO.initialize(GLConfig::LIGHT_COLOR_INTENSITIES_UBO);
	m_clusteredShadingGlobalsUBO.initialize(GLConfig::CLUSTERED_SHADING_GLOBALS_UBO);

	m_modelShader.initialize(MODEL_VERT_SHADER_PATH, MODEL_FRAG_SHADER_PATH, &GLConfig::getGlobalShaderDefines());
	m_combineShader.initialize(COMBINE_VERT_SHADER_PATH, COMBINE_FRAG_SHADER_PATH, &GLConfig::getGlobalShaderDefines());
	m_skyboxShader.initialize(MODEL_VERT_SHADER_PATH, SKYBOX_FRAG_SHADER_PATH, &GLConfig::getGlobalShaderDefines());

	LightingGlobalsUBO* lightingGlobals = (LightingGlobalsUBO*) m_lightningGlobalsUBO.mapBuffer();
	lightingGlobals->u_ambient = AMBIENT;
	m_lightningGlobalsUBO.unmapBuffer();

	ClusteredShading::GlobalsUBO* clusteredShadingGlobals = (ClusteredShading::GlobalsUBO*) m_clusteredShadingGlobalsUBO.mapBuffer();
	*clusteredShadingGlobals = m_clusteredShading.getUBOData();
	m_clusteredShadingGlobalsUBO.unmapBuffer();

	m_whiteTexture.initialize(DBTexture(WHITE_TEX_PATH));
	m_blackTexture.initialize(DBTexture(BLACK_TEX_PATH));
	m_dfvTexture.initialize(DBTexture(DFV_TEX_PATH, DBTexture::EFormat::FLOAT), 0, GLTexture::ETextureMinFilter::LINEAR, GLTexture::ETextureMagFilter::LINEAR);

	m_lightGridTextureBuffer.initialize(m_clusteredShading.getGridSize() * sizeof(glm::uvec2), GLTextureBuffer::ESizedFormat::RG32I, GLTextureBuffer::EDrawUsage::STREAM);
	m_lightIndiceTextureBuffer.initialize(m_clusteredShading.getMaxNumLightIndices() * sizeof(ushort), GLTextureBuffer::ESizedFormat::R16I, GLTextureBuffer::EDrawUsage::STREAM);
}

void GLRenderer::render(const PerspectiveCamera& a_camera, const LightManager& a_lightManager)
{
	GLEngine::graphics->clear(glm::vec4(0.8f, 0.8f, 0.8f, 1.0f));

	// UPDATE CAMERA DATA //
	CameraVarsUBO* cameraVars = (CameraVarsUBO*) m_cameraVarsUBO.mapBuffer();
	cameraVars->u_vpMatrix = a_camera.getCombinedMatrix();
	cameraVars->u_viewMatrix = a_camera.getViewMatrix();
	cameraVars->u_normalMatrix = glm::inverse(glm::transpose(a_camera.getViewMatrix()));
	cameraVars->u_eyePos = glm::vec3(a_camera.getViewMatrix() * glm::vec4(a_camera.getPosition(), 1.0));
	m_cameraVarsUBO.unmapBuffer();

	// UPDATE CLUSTERED SHADING STUFF //
	const glm::mat4& viewMatrix = a_camera.getViewMatrix();
	const glm::vec4* lightPositionRanges = a_lightManager.getLightPositionRanges();
	glm::vec4* viewspaceLightPositionRanges = (glm::vec4*) m_lightPositionRangesUBO.mapBuffer();
	for (uint i = 0; i < a_lightManager.getNumLights(); ++i)
	{
		viewspaceLightPositionRanges[i] = viewMatrix * glm::vec4(glm::vec3(lightPositionRanges[i]), 1.0);
		viewspaceLightPositionRanges[i].w = lightPositionRanges[i].w;
	}
	m_clusteredShading.update(a_camera, a_lightManager.getNumLights(), viewspaceLightPositionRanges);
	m_lightPositionRangesUBO.unmapBuffer();
	m_lightColorIntensitiesUBO.upload(sizeof(glm::vec4) * a_lightManager.getNumLights(), &a_lightManager.getLightColorIntensities()[0]);
	m_lightGridTextureBuffer.upload(m_clusteredShading.getGridSize() * sizeof(glm::uvec2), m_clusteredShading.getLightGrid());
	m_lightIndiceTextureBuffer.upload(m_clusteredShading.getNumLightIndices() * sizeof(ushort), m_clusteredShading.getLightIndices());
	
	// BIND CLUSTERED SHADING TEXTURES
	m_dfvTexture.bind(GLConfig::DFV_TEXTURE_BINDING_POINT);
	m_lightGridTextureBuffer.bind(GLConfig::LIGHT_GRID_TEXTURE_BINDING_POINT);
	m_lightIndiceTextureBuffer.bind(GLConfig::LIGHT_INDICE_TEXTURE_BINDING_POINT);

	m_sceneFBO.begin();
	GLEngine::graphics->clear(glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));

	// RENDER SKYBOXES //
	GLEngine::graphics->setDepthWrite(false);
	m_skyboxShader.begin();
	for (GLScene* scene : m_skyboxScenes)
	{
		scene->setTranslation(a_camera.getPosition());
		scene->render(m_modelMatrixUBO);
	}
	m_skyboxShader.end();
	GLEngine::graphics->setDepthWrite(true);

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
