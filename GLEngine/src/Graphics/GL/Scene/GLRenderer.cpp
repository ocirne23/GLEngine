#include "Graphics/GL/Scene/GLRenderer.h"

#include "Database/Assets/DBScene.h"
#include "Database/Assets/DBTexture.h"
#include "GLEngine.h"
#include "Graphics/GL/Scene/GLConfig.h"
#include "Graphics/GL/Scene/GLRenderObject.h"
#include "Graphics/GL/Tech/QuadDrawer.h"
#include "Graphics/Graphics.h"
#include "Graphics/Utils/PerspectiveCamera.h"
#include "Graphics/Utils/LightManager.h"

#include <glm/gtc/matrix_transform.hpp>

BEGIN_UNNAMED_NAMESPACE()

const char* const DFV_TEX_PATH = "Assets/Utils/ggx-helper-dfv.hdr";

const char* const MODEL_VERT_SHADER_PATH = "Shaders/modelshader.vert";
const char* const MODEL_FRAG_SHADER_PATH = "Shaders/modelshader.frag";

const char* const SKYBOX_VERT_SHADER_PATH = "Shaders/skyboxshader.vert";
const char* const SKYBOX_FRAG_SHADER_PATH = "Shaders/skyboxshader.frag";

const char* const DEPTH_VERT_SHADER_PATH = "Shaders/depth.vert";
const char* const DEPTH_FRAG_SHADER_PATH = "Shaders/depth.frag";

const char* const COMBINE_VERT_SHADER_PATH = "Shaders/quad.vert";
const char* const COMBINE_FRAG_SHADER_PATH = "Shaders/combine.frag";

const char* const FXAA_VERT_SHADER_PATH = "Shaders/FXAA/FXAA.vert";
const char* const FXAA_FRAG_SHADER_PATH = "Shaders/FXAA/FXAA_High_Quality.frag";

const glm::vec3 AMBIENT(0.125f);
const glm::ivec2 SHADOW_MAP_RES(8192);
const glm::ivec2 CUBE_MAP_RES(4096);
const float SHADOW_VIEW_RANGE = 200.0f;
const float SUN_DISTANCE = 50.0f;

END_UNNAMED_NAMESPACE()

void GLRenderer::initialize(const PerspectiveCamera& a_camera)
{
	uint screenWidth = GLEngine::graphics->getViewportWidth();
	uint screenHeight = GLEngine::graphics->getViewportHeight();

	m_sceneFBO.initialize(GLConfig::getMultisampleType());
	m_sceneFBO.addFramebufferTexture(GLFramebuffer::ESizedFormat::RGB32F, GLFramebuffer::EAttachment::COLOR0, screenWidth, screenHeight);
	m_sceneFBO.setDepthbufferTexture(GLFramebuffer::ESizedFormat::DEPTH24, screenWidth, screenHeight);

	m_shadowFBO.initialize();
	m_shadowFBO.setDepthbufferTexture(GLFramebuffer::ESizedFormat::DEPTH32, SHADOW_MAP_RES.x, SHADOW_MAP_RES.y, GLFramebuffer::ETextureMagFilter::LINEAR, GLFramebuffer::ETextureMinFilter::LINEAR);
	m_shadowCamera.initialize(SHADOW_VIEW_RANGE, SHADOW_VIEW_RANGE, 90.0f, 0.1f, SHADOW_VIEW_RANGE, PerspectiveCamera::EProjection::ORTHOGRAPHIC);

	m_clusteredShading.initialize(a_camera, screenWidth, screenHeight);
	m_hbao.initialize(a_camera, screenWidth, screenHeight);
	m_bloom.initialize(screenWidth, screenHeight);
	m_fxaa.initialize(FXAA::EQuality::EXTREME, screenWidth, screenHeight);

	m_modelDataUBO.initialize(GLConfig::getUBOConfig(GLConfig::EUBOs::ModelData));
	m_cameraVarsUBO.initialize(GLConfig::getUBOConfig(GLConfig::EUBOs::CameraVars));
	m_lightningGlobalsUBO.initialize(GLConfig::getUBOConfig(GLConfig::EUBOs::LightingGlobals));
	m_settingsGlobalsUBO.initialize(GLConfig::getUBOConfig(GLConfig::EUBOs::SettingsGlobals));

	DBTexture dfvDBTexture;
	dfvDBTexture.loadFromFile(DFV_TEX_PATH, DBTexture::EFormat::FLOAT);
	m_dfvTexture.initialize(dfvDBTexture, 0, GLTexture::ETextureMinFilter::LINEAR, GLTexture::ETextureMagFilter::LINEAR);

	updateSettingsGlobalsUBO();

	reloadShaders();
}

void GLRenderer::reloadShaders()
{
	m_depthPrepassShader.initialize(DEPTH_VERT_SHADER_PATH, DEPTH_FRAG_SHADER_PATH, &GLConfig::getGlobalShaderDefines());
	m_modelShader.initialize(MODEL_VERT_SHADER_PATH, MODEL_FRAG_SHADER_PATH, &GLConfig::getGlobalShaderDefines());
	m_skyboxShader.initialize(SKYBOX_VERT_SHADER_PATH, SKYBOX_FRAG_SHADER_PATH, &GLConfig::getGlobalShaderDefines());
	m_combineShader.initialize(COMBINE_VERT_SHADER_PATH, COMBINE_FRAG_SHADER_PATH, &GLConfig::getGlobalShaderDefines());

	m_hbao.reloadShader();
	m_bloom.reloadShader();
	QuadDrawer::reloadShader();
}

void GLRenderer::render(const PerspectiveCamera& a_camera, const LightManager& a_lightManager)
{
	m_sceneCamera = &a_camera;
	uint screenWidth = GLEngine::graphics->getViewportWidth();
	uint screenHeight = GLEngine::graphics->getViewportHeight();
	
	m_clusteredShading.update(a_camera, a_lightManager);
	m_clusteredShading.bindTextureBuffers();
	m_dfvTexture.bind(GLConfig::getTextureBindingPoint(GLConfig::ETextures::DFVTexture));

	updateLightingGlobalsUBO(a_camera);

	// Shadows and depth prepass first
	GLEngine::graphics->setDepthWrite(true);
	GLEngine::graphics->setColorWrite(false);
	GLEngine::graphics->setDepthTest(true);
	GLEngine::graphics->setDepthFunc(Graphics::EDepthFunc::LESS);
	
	// SUN SHADOW MAP GENERATION //
	if (m_shadowsEnabled)
	{
		m_shadowFBO.begin();
		m_sceneCamera = &m_shadowCamera;
		updateCameraDataUBO(m_shadowCamera);
		GLEngine::graphics->setViewportSize(SHADOW_MAP_RES.x, SHADOW_MAP_RES.y);
		GLEngine::graphics->setFaceCulling(Graphics::EFaceCulling::FRONT);
		GLEngine::graphics->clearDepthOnly();
		m_depthPrepassShader.begin();
		for (GLRenderObject* renderObject : m_renderObjects)
			renderObject->render(*this, true);
		m_depthPrepassShader.end();
		GLEngine::graphics->setFaceCulling(Graphics::EFaceCulling::BACK);
		m_sceneCamera = &a_camera;
		m_shadowFBO.end();
	}
	else
	{
		m_shadowFBO.begin();
		GLEngine::graphics->clearDepthOnly();
		m_shadowFBO.end();
	}
	
	// MAIN SCENE //
	m_sceneFBO.begin();
	GLEngine::graphics->setViewportSize(screenWidth, screenHeight);
	GLEngine::graphics->clearDepthOnly();
	updateCameraDataUBO(a_camera);

	// DEPTH PREPASS //
	m_depthPrepassShader.begin();
	for (GLRenderObject* renderObject : m_renderObjects)
		renderObject->render(*this, true);
	m_depthPrepassShader.end();
	
	GLEngine::graphics->setDepthFunc(Graphics::EDepthFunc::LEQUAL);
	GLEngine::graphics->setDepthWrite(false);
	GLEngine::graphics->setColorWrite(true);

	// RENDER SKYBOXES // TODO: render skyboxes last with depth test to reduce fillrate
	GLEngine::graphics->setDepthTest(false);
	m_skyboxShader.begin();
	for (GLRenderObject* renderObject : m_skyboxObjects)
		renderObject->render(*this, false);
	m_skyboxShader.end();
	
	// RENDER MODELS //
	GLEngine::graphics->setDepthTest(true);
	m_modelShader.begin();
	m_shadowFBO.bindDepthTexture(GLConfig::getTextureBindingPoint(GLConfig::ETextures::SunShadow));
	for (GLRenderObject* renderObject : m_renderObjects)
		renderObject->render(*this, false);
	m_modelShader.end();

	m_sceneFBO.end();
	m_sceneFBO.bindTexture(0, GLConfig::getTextureBindingPoint(GLConfig::ETextures::Color));

	GLEngine::graphics->setDepthTest(false);
	m_combineShader.begin();
	QuadDrawer::drawQuad(m_combineShader);
	m_combineShader.end();

	// APPLY HBAO //
	GLFramebuffer* hbaoResult = NULL;
	if (m_hbaoEnabled)
		hbaoResult = &m_hbao.getHBAOResultFBO(m_sceneFBO);

	// APPLY BLOOM // 
	GLFramebuffer* bloomResult = NULL;
	if (m_bloomEnabled)
		bloomResult = &m_bloom.getBloomResultFBO(m_sceneFBO);

	// BIND RESULT TEXTURES OF ALL PASSES // 
	if (m_hbaoEnabled)
		hbaoResult->bindTexture(0, GLConfig::getTextureBindingPoint(GLConfig::ETextures::HBAOResult));

	if (m_bloomEnabled)
		bloomResult->bindTexture(0, GLConfig::getTextureBindingPoint(GLConfig::ETextures::BloomResult));

	// APPLY FXAA AND RENDER COMBINED SCENE //
	if (m_fxaaEnabled)
		m_fxaa.begin();
	m_combineShader.begin();
	QuadDrawer::drawQuad(m_combineShader);
	m_combineShader.end();
	if (m_fxaaEnabled)
		m_fxaa.endAndRender();

	GLEngine::graphics->setDepthTest(true);
	m_sceneCamera = NULL;
}

void GLRenderer::addRenderObject(GLRenderObject* a_renderObject)
{
	m_renderObjects.push_back(a_renderObject);
}

void GLRenderer::removeRenderObject(GLRenderObject* a_renderObject)
{
	auto it = eastl::find(m_renderObjects.begin(), m_renderObjects.end(), a_renderObject);
	if (it != m_renderObjects.end())
		m_renderObjects.erase(it);
}

void GLRenderer::addSkybox(GLRenderObject* a_renderObject)
{
	m_skyboxObjects.push_back(a_renderObject);
}

void GLRenderer::removeSkybox(GLRenderObject* a_renderObject)
{
	auto it = eastl::find(m_skyboxObjects.begin(), m_skyboxObjects.end(), a_renderObject);
	if (it != m_skyboxObjects.end())
		m_skyboxObjects.erase(it);
}

void GLRenderer::setHBAOEnabled(bool a_enabled)
{
	m_hbaoEnabled = a_enabled;
	updateSettingsGlobalsUBO();
}

void GLRenderer::setBloomEnabled(bool a_enabled)
{
	m_bloomEnabled = a_enabled;
	updateSettingsGlobalsUBO();
}

void GLRenderer::setShadowsEnabled(bool a_enabled)
{
	m_shadowsEnabled = a_enabled;
	updateSettingsGlobalsUBO();
}

void GLRenderer::setSun(const glm::vec3& a_direction, const glm::vec3& a_color, float a_intensity)
{
	m_sunDir = a_direction;
	m_sunColorIntensity = glm::vec4(a_color, a_intensity);

	// TODO: more efficient camera positioning
	m_shadowCamera.setPosition(glm::vec3(a_direction * SUN_DISTANCE));
	m_shadowCamera.lookAtPoint(glm::vec3(0));
	m_shadowCamera.updateMatrices();
}

void GLRenderer::setModelDataUBO(const ModelData& a_modelData)
{
	m_modelDataUBO.upload(sizeof(ModelData), &a_modelData);
}

void GLRenderer::drawDebugSphere(const glm::vec3& position, float radius)
{
	if (!m_debugSphere.isInitialized())
		m_debugSphere.initialize(DBScene("assets/Models/sphere/sphere.obj"));

	ModelData data;
	data.u_modelMatrix = glm::translate(glm::mat4(), position);
	data.u_modelMatrix = glm::scale(data.u_modelMatrix, glm::vec3(radius));
	data.u_normalMatrix = glm::inverse(glm::transpose(data.u_modelMatrix * m_sceneCamera->getViewMatrix()));

	setModelDataUBO(data);
	m_debugSphere.m_meshes[0].render();
}

void GLRenderer::updateLightingGlobalsUBO(const PerspectiveCamera& a_camera)
{
	LightingGlobalsData* lightingGlobals = rcast<LightingGlobalsData*>(m_lightningGlobalsUBO.mapBuffer());
	lightingGlobals->u_ambient = AMBIENT;
	lightingGlobals->u_sunDir = glm::normalize(glm::mat3(a_camera.getViewMatrix()) * m_sunDir);
	lightingGlobals->u_sunColorIntensity = m_sunColorIntensity;
	lightingGlobals->u_cubemapPos = m_cubeMap ? m_cubeMap->getPosition() : glm::vec3(0);
	static const glm::mat4 biasMatrix(
		0.5, 0.0, 0.0, 0.0,
		0.0, 0.5, 0.0, 0.0,
		0.0, 0.0, 0.5, 0.0,
		0.5, 0.5, 0.5, 1.0);
	lightingGlobals->u_shadowMat = biasMatrix * m_shadowCamera.getCombinedMatrix();
	m_lightningGlobalsUBO.unmapBuffer();
}

void GLRenderer::updateCameraDataUBO(const PerspectiveCamera& a_camera)
{
	CameraVarsData* cameraVars = rcast<CameraVarsData*>(m_cameraVarsUBO.mapBuffer());
	cameraVars->u_vpMatrix     = a_camera.getCombinedMatrix();
	cameraVars->u_viewMatrix   = a_camera.getViewMatrix();
	cameraVars->u_eyePos       = glm::vec3(a_camera.getViewMatrix() * glm::vec4(a_camera.getPosition(), 1.0));
	cameraVars->u_wsEyePos     = a_camera.getPosition();
	cameraVars->u_camNear      = a_camera.getNear();
	cameraVars->u_camFar       = a_camera.getFar();
	m_cameraVarsUBO.unmapBuffer();
}

void GLRenderer::updateSettingsGlobalsUBO()
{
	SettingsGlobalsData* settings = rcast<SettingsGlobalsData*>(m_settingsGlobalsUBO.mapBuffer());
	settings->u_hbaoEnabled       = m_hbaoEnabled;
	settings->u_bloomEnabled      = m_bloomEnabled;
	settings->u_shadowsEnabled    = m_shadowsEnabled;
	m_settingsGlobalsUBO.unmapBuffer();
}

/*
if (!m_cubeMapGenerated)
{
GLEngine::graphics->setDepthFunc(Graphics::EDepthFunc::LESS);
GLEngine::graphics->setFaceCulling(Graphics::EFaceCulling::BACK);

for (uint i = 0; i < 6; ++i)
{
const PerspectiveCamera& cam = m_cubeMapGenerator.beginRenderCubeMapFace(a_camera.getPosition(), CubeMapGen::ECubeMapFace(i));
updateLightingGlobalsUBO(cam);
updateCameraDataUBO(cam);

GLEngine::graphics->setDepthWrite(false);
GLEngine::graphics->setDepthTest(false);

m_skyboxShader.begin();
for (GLRenderObject* renderObject : m_skyboxObjects)
renderObject->render(*this, false);
m_skyboxShader.end();

GLEngine::graphics->setDepthWrite(true);
GLEngine::graphics->setDepthTest(true);
GLEngine::graphics->clearDepthOnly();

m_modelShader.begin();
m_shadowFBO.bindDepthTexture(GLConfig::getTextureBindingPoint(GLConfig::ETextures::SunShadow));
for (GLRenderObject* renderObject : m_renderObjects)
renderObject->render(*this, false);
m_modelShader.end();

m_cubeMapGenerator.endRenderCubeMapFace();
}
m_cubeMap = &m_cubeMapGenerator.getCubeMap();
m_cubeMap->bind(12);
m_cubeMapGenerated = true;
}*/