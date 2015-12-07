#include "Graphics/GL/Scene/GLRenderer.h"

#include "Database/Assets/DBTexture.h"
#include "Graphics/GL/Scene/GLScene.h"
#include "Graphics/GL/Scene/GLConfig.h"
#include "Graphics/Utils/PerspectiveCamera.h"
#include "Graphics/Utils/LightManager.h"
#include "GLEngine.h"
#include "Graphics/Graphics.h"
#include "EASTL/to_string.h"

BEGIN_UNNAMED_NAMESPACE()

static const char* const DFV_TEX_PATH = "Assets/Utils/ggx-helper-dfv.hdr";
static const char* const MODEL_VERT_SHADER_PATH = "Shaders/modelshader.vert";
static const char* const MODEL_FRAG_SHADER_PATH = "Shaders/modelshader.frag";
static const uint CLUSTERED_SHADING_TILE_WIDTH_PX = 64;
static const uint CLUSTERED_SHADING_TILE_HEIGHT_PX = 64;

static const glm::vec3 AMBIENT(0.15f);

END_UNNAMED_NAMESPACE()

void GLRenderer::initialize(const PerspectiveCamera& a_camera)
{
	uint screenWidth = GLEngine::graphics->getViewportWidth();
	uint screenheight = GLEngine::graphics->getViewportHeight();
	m_hbao.initialize(a_camera, screenWidth, screenheight, GLFramebuffer::EMultiSampleType::MSAA_4X);
	m_clusteredShading.initialize(a_camera, screenWidth, screenheight, CLUSTERED_SHADING_TILE_WIDTH_PX, CLUSTERED_SHADING_TILE_HEIGHT_PX);

	m_modelMatrixUBO.initialize(GLConfig::MODEL_MATRIX_UBO);
	m_cameraVarsUBO.initialize(GLConfig::CAMERA_VARS_UBO);
	m_lightningGlobalsUBO.initialize(GLConfig::LIGHTING_GLOBALS_UBO);
	m_lightPositionRangesUBO.initialize(GLConfig::LIGHT_POSITION_RANGES_UBO);
	m_lightColorIntensitiesUBO.initialize(GLConfig::LIGHT_COLOR_INTENSITIES_UBO);
	m_clusteredShadingGlobalsUBO.initialize(GLConfig::CLUSTERED_SHADING_GLOBALS_UBO);

	eastl::vector<eastl::string> defines;
	defines.push_back(eastl::string("MAX_LIGHTS ").append(StringUtils::to_string((int) GLConfig::MAX_LIGHTS)));
	defines.push_back(eastl::string("LIGHT_GRID_WIDTH ").append(StringUtils::to_string(m_clusteredShading.getGridWidth())));
	defines.push_back(eastl::string("LIGHT_GRID_HEIGHT ").append(StringUtils::to_string(m_clusteredShading.getGridHeight())));
	defines.push_back(eastl::string("LIGHT_GRID_DEPTH ").append(StringUtils::to_string(m_clusteredShading.getGridDepth())));
	defines.push_back(eastl::string("LIGHT_GRID_TILE_WIDTH ").append(StringUtils::to_string(m_clusteredShading.getTileWidth())));
	defines.push_back(eastl::string("LIGHT_GRID_TILE_HEIGHT ").append(StringUtils::to_string(m_clusteredShading.getTileHeight())));

	m_modelShader.initialize(MODEL_VERT_SHADER_PATH, MODEL_FRAG_SHADER_PATH, &defines);
	m_modelShader.begin();
	m_modelShader.setUniform1i("u_textureAtlasArray", 1);
	m_modelShader.setUniform1i("u_dfvTexture", 2);
	m_modelShader.setUniform1i("u_lightGrid", 3);
	m_modelShader.setUniform1i("u_lightIndices", 4);
	m_modelShader.end();

	LightingGlobalsUBO* lightingGlobals = (LightingGlobalsUBO*) m_lightningGlobalsUBO.mapBuffer();
	lightingGlobals->u_ambient = AMBIENT;
	m_lightningGlobalsUBO.unmapBuffer();

	ClusteredShading::GlobalsUBO* clusteredShadingGlobals = (ClusteredShading::GlobalsUBO*) m_clusteredShadingGlobalsUBO.mapBuffer();
	*clusteredShadingGlobals = m_clusteredShading.getUBOData();
	m_clusteredShadingGlobalsUBO.unmapBuffer();

	DBTexture tex(DFV_TEX_PATH, DBTexture::EFormat::FLOAT);
	m_dfvTexture.initialize(tex, 0, 
						    GLTexture::ETextureMinFilter::LINEAR, GLTexture::ETextureMagFilter::LINEAR,
					        GLTexture::ETextureWrap::CLAMP_TO_EDGE, GLTexture::ETextureWrap::CLAMP_TO_EDGE);

	m_lightGridTextureBuffer.initialize(GLTextureBuffer::ESizedFormat::RG32I, GLTextureBuffer::EDrawUsage::STREAM);
	m_lightIndiceTextureBuffer.initialize(GLTextureBuffer::ESizedFormat::R16I, GLTextureBuffer::EDrawUsage::STREAM);
}

void GLRenderer::render(const PerspectiveCamera& a_camera, const LightManager& a_lightManager)
{
	if (m_hbaoEnabled)
	{
		m_hbao.begin();
		GLEngine::graphics->clear(glm::vec4(0.8f, 0.8f, 0.8f, 1.0f));
	}

	// Update camera data
	CameraVarsUBO* cameraVars = (CameraVarsUBO*) m_cameraVarsUBO.mapBuffer();
	cameraVars->u_vpMatrix = a_camera.getCombinedMatrix();
	cameraVars->u_viewMatrix = a_camera.getViewMatrix();
	cameraVars->u_normalMatrix = glm::inverse(glm::transpose(a_camera.getViewMatrix()));
	cameraVars->u_eyePos = glm::vec3(a_camera.getViewMatrix() * glm::vec4(a_camera.getPosition(), 1.0));
	m_cameraVarsUBO.unmapBuffer();

	{ // Clustered Shading stuff
		// Transform positions to viewspace
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
	}

	m_dfvTexture.bind(2);
	m_lightGridTextureBuffer.bind(3);
	m_lightIndiceTextureBuffer.bind(4);

	m_modelShader.begin();

	for (GLScene* scene : m_scenes)
		scene->render(m_modelMatrixUBO);

	m_modelShader.end();

	if (m_hbaoEnabled)
		m_hbao.endAndRender();
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
