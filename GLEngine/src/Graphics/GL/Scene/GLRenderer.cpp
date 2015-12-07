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

static const glm::vec3 AMBIENT(0.15f);

END_UNNAMED_NAMESPACE()

void GLRenderer::initialize(const PerspectiveCamera& a_camera)
{
	uint screenWidth = GLEngine::graphics->getViewportWidth();
	uint screenheight = GLEngine::graphics->getViewportHeight();

	m_hbao.initialize(a_camera, screenWidth, screenheight, GLConfig::MULTISAMPLE_TYPE);
	m_clusteredShading.initialize(a_camera, screenWidth, screenheight, GLConfig::CLUSTERED_SHADING_TILE_WIDTH, GLConfig::CLUSTERED_SHADING_TILE_HEIGHT);

	m_modelMatrixUBO.initialize(GLConfig::MODEL_MATRIX_UBO);
	m_cameraVarsUBO.initialize(GLConfig::CAMERA_VARS_UBO);
	m_lightningGlobalsUBO.initialize(GLConfig::LIGHTING_GLOBALS_UBO);
	m_lightPositionRangesUBO.initialize(GLConfig::LIGHT_POSITION_RANGES_UBO);
	m_lightColorIntensitiesUBO.initialize(GLConfig::LIGHT_COLOR_INTENSITIES_UBO);
	m_clusteredShadingGlobalsUBO.initialize(GLConfig::CLUSTERED_SHADING_GLOBALS_UBO);

	m_modelShader.initialize(MODEL_VERT_SHADER_PATH, MODEL_FRAG_SHADER_PATH, &GLConfig::getGlobalShaderDefines());

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

	m_dfvTexture.bind(GLConfig::DFV_TEXTURE_BINDING_POINT);
	m_lightGridTextureBuffer.bind(GLConfig::LIGHT_GRID_TEXTURE_BINDING_POINT);
	m_lightIndiceTextureBuffer.bind(GLConfig::LIGHT_INDICE_TEXTURE_BINDING_POINT);

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
