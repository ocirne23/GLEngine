#include "Graphics/GL/Tech/HBAO.h"

#include "Database/Assets/DBTexture.h"
#include "Graphics/GL/Scene/GLConfig.h"
#include "Graphics/GL/Tech/QuadDrawer.h"
#include "Graphics/Utils/PerspectiveCamera.h"
#include "Graphics/Utils/CheckGLError.h"
#include "Utils/StringUtils.h"

#include <glm/gtc/random.hpp>

BEGIN_UNNAMED_NAMESPACE()

const char* const QUAD_VERT_SHADER_PATH = "Shaders/quad.vert";
const char* const HBAO_FRAG_SHADER_PATH = "Shaders/HBAO/HBAO.frag";

const float RES_RATIO          = 1.0f;
const float AO_RADIUS          = 0.30f; // In meters
const uint NUM_DIRS            = 6;
const uint NUM_STEPS           = 6;
const float AO_STRENGTH        = 0.7f;
const float AO_ANGLE_BIAS_DEG  = 30.0f;
const uint NOISE_TEXTURE_RES   = 8;
const uint BLUR_RADIUS         = 8;
const float MAX_RADIUS_PERCENT = 0.2f; // Max sample distance based on screen size

END_UNNAMED_NAMESPACE()

void HBAO::initialize(const PerspectiveCamera& a_camera, uint a_xRes, uint a_yRes)
{
	const uint screenWidth = a_xRes;
	const uint screenHeight = a_yRes;
	const float aoWidth = screenWidth / RES_RATIO;
	const float aoHeight = screenHeight / RES_RATIO;

	m_hbaoResultFBO.initialize(GLConfig::getMultisampleType());
	m_hbaoResultFBO.addFramebufferTexture(GLFramebuffer::ESizedFormat::R8, GLFramebuffer::EAttachment::COLOR0, screenWidth, screenHeight);

	uint noiseTexWidth = NOISE_TEXTURE_RES;
	uint noiseTexHeight = NOISE_TEXTURE_RES;
	float *noise = new float[noiseTexWidth * noiseTexHeight * 4];
	for (uint y = 0; y < noiseTexHeight; ++y)
	{
		for (uint x = 0; x < noiseTexWidth; ++x)
		{
			glm::vec2 xy = glm::circularRand(1.0f);
			float z = glm::linearRand(0.0f, 1.0f);
			float w = glm::linearRand(0.0f, 1.0f);

			int offset = 4 * (y * noiseTexWidth + x);
			noise[offset + 0] = xy[0];
			noise[offset + 1] = xy[1];
			noise[offset + 2] = z;
			noise[offset + 3] = w;
		}
	}
	
	m_noiseTexture.initialize(DBTexture(noiseTexWidth, noiseTexHeight, 4, DBTexture::EFormat::FLOAT, (byte*) noise), 0,
	                          GLTexture::ETextureMinFilter::NEAREST, GLTexture::ETextureMagFilter::NEAREST,
	                          GLTexture::ETextureWrap::REPEAT, GLTexture::ETextureWrap::REPEAT);

	float fovRad = glm::radians(a_camera.getVFov());
	float near = a_camera.getNear();
	float far = a_camera.getFar();

	GlobalsUBO globals;
	globals.fullResolution    = glm::vec2(screenWidth, screenHeight);
	globals.invFullResolution = 1.0f / globals.fullResolution;
	globals.aoResolution      = glm::vec2(aoWidth, aoHeight);
	globals.invAOResolution   = 1.0f / globals.aoResolution;
	globals.focalLen.x        = 1.0f / tanf(fovRad * 0.5f) * (aoHeight / aoWidth);
	globals.focalLen.y        = 1.0f / tanf(fovRad * 0.5f);
	globals.invFocalLen       = 1.0f / globals.focalLen;
	globals.uvToViewA         = -2.0f * globals.invFocalLen;
	globals.uvToViewB         = 1.0f * globals.invFocalLen;
	globals.r                 = AO_RADIUS;
	globals.r2                = globals.r * globals.r;
	globals.negInvR2          = -1.0f / globals.r2;
	globals.maxRadiusPixels   = MAX_RADIUS_PERCENT * glm::min(globals.fullResolution.x, globals.fullResolution.y);
	globals.angleBias         = glm::radians(AO_ANGLE_BIAS_DEG);
	globals.tanAngleBias      = tanf(globals.angleBias);
	globals.strength          = AO_STRENGTH;
	globals.padding           = 0.0f;
	globals.noiseTexScale     = glm::vec2(screenWidth / (float) noiseTexWidth, screenHeight / (float) noiseTexHeight);
	globals.ndcDepthConv.x    = (near - far) / (2.0f * near * far);
	globals.ndcDepthConv.y    = (near + far) / (2.0f * near * far);

	m_hbaoGlobalsBuffer.initialize(GLConfig::getUBOConfig(GLConfig::EUBOs::HBAOGlobals));
	m_hbaoGlobalsBuffer.upload(sizeof(GlobalsUBO), &globals);

	m_bilateralBlur.initialize(BilateralBlur::EBlurValueType::FLOAT, screenWidth, screenHeight, BLUR_RADIUS);
	reloadShader();

	m_initialized = true;
}

void HBAO::reloadShader()
{
	eastl::vector<eastl::string> defines = GLConfig::getGlobalShaderDefines();
	defines.push_back("NUM_DIRECTIONS " + StringUtils::to_string(NUM_DIRS));
	defines.push_back("NUM_STEPS " + StringUtils::to_string(NUM_STEPS));

	m_hbaoFullShader.initialize(QUAD_VERT_SHADER_PATH, HBAO_FRAG_SHADER_PATH, &defines);
	m_bilateralBlur.reloadShader();
}

GLFramebuffer& HBAO::getHBAOResultFBO(GLFramebuffer& a_sceneFBO)
{
	assert(m_initialized);

	// Apply HBAO //
	a_sceneFBO.bindDepthTexture(GLConfig::getTextureBindingPoint(GLConfig::ETextures::Depth));
	m_noiseTexture.bind(GLConfig::getTextureBindingPoint(GLConfig::ETextures::HBAONoise));
	m_hbaoGlobalsBuffer.bind();

	m_hbaoResultFBO.begin();
	QuadDrawer::drawQuad(m_hbaoFullShader);
	m_hbaoResultFBO.end();

	m_bilateralBlur.blurFBO(m_hbaoResultFBO, a_sceneFBO);

	return m_hbaoResultFBO;
}