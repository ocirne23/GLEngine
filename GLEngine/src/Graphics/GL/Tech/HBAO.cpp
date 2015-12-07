#include "Graphics/GL/Tech/HBAO.h"

#include "GLEngine.h"
#include "Graphics/Graphics.h"
#include "Graphics/Utils/PerspectiveCamera.h"
#include "Graphics/GL/GL.h"
#include "Graphics/GL/Tech/QuadDrawer.h"
#include "Graphics/Utils/CheckGLError.h"
#include "Database/Assets/DBTexture.h"
#include "Graphics/GL/Scene/GLConfig.h"

#include <glm/glm.hpp>
#include <glm/gtx/random.hpp>

BEGIN_UNNAMED_NAMESPACE()

static const float RES_RATIO = 1.0f;
static const float AO_RADIUS = 0.15f; // In meters
static const uint AO_DIRS = 6;
static const uint AO_SAMPLES = 6;
static const float AO_STRENGTH = 1.0f;
static const float AO_ANGLE_BIAS_DEG = 30.0f;
static const uint NOISE_RES = 8;

END_UNNAMED_NAMESPACE()

void HBAO::initialize(const PerspectiveCamera& a_camera, uint a_xRes, uint a_yRes, GLFramebuffer::EMultiSampleType a_multisampleType)
{
	const uint screenWidth = a_xRes;
	const uint screenHeight = a_yRes;
	const float aoWidth = screenWidth / RES_RATIO;
	const float aoHeight = screenHeight / RES_RATIO;

	m_fboFullRes.initialize(a_multisampleType);
	m_blurXFbo.initialize();
	m_blurYFbo.initialize();

	m_fboFullRes.setDepthbufferTexture(GLFramebuffer::ESizedFormat::DEPTH32, screenWidth, screenHeight);
	m_fboFullRes.addFramebufferTexture(GLFramebuffer::ESizedFormat::RGB8, GLFramebuffer::EAttachment::COLOR0, screenWidth, screenHeight);
	m_blurXFbo.addFramebufferTexture(GLFramebuffer::ESizedFormat::RG16F, GLFramebuffer::EAttachment::COLOR0, screenWidth, screenHeight);
	m_blurYFbo.addFramebufferTexture(GLFramebuffer::ESizedFormat::RG16F, GLFramebuffer::EAttachment::COLOR0, screenWidth, screenHeight);
	CHECK_GL_ERROR();

	m_hbaoFullShader.initialize("Shaders/quad.vert", "Shaders/HBAO/HBAO.frag", &GLConfig::getGlobalShaderDefines());
	m_blurXShader.initialize("Shaders/quad.vert", "Shaders/HBAO/blurx.frag", &GLConfig::getGlobalShaderDefines());
	m_blurYShader.initialize("Shaders/quad.vert", "Shaders/HBAO/blury.frag", &GLConfig::getGlobalShaderDefines());

	uint noiseTexWidth = NOISE_RES;
	uint noiseTexHeight = NOISE_RES;
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
	DBTexture tex(noiseTexWidth, noiseTexHeight, 4, DBTexture::EFormat::FLOAT, (byte*) noise);
	
	m_noiseTexture.initialize(tex, 0,
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
	globals.maxRadiusPixels   = 0.1f * glm::min(globals.fullResolution.x, globals.fullResolution.y);
	globals.angleBias         = glm::radians(AO_ANGLE_BIAS_DEG);
	globals.tanAngleBias      = tanf(globals.angleBias);
	globals.strength          = AO_STRENGTH;
	globals.padding           = 0.0f;
	globals.noiseTexScale     = glm::vec2(screenWidth / (float) noiseTexWidth, screenHeight / (float) noiseTexHeight);
	globals.ndcDepthConv.x    = (near - far) / (2.0f * near * far);
	globals.ndcDepthConv.y    = (near + far) / (2.0f * near * far);

	m_hbaoFullShader.begin();
	m_hbaoGlobalsBuffer.initialize(GLConfig::HBAO_GLOBALS_UBO);
	m_hbaoGlobalsBuffer.upload(sizeof(GlobalsUBO), &globals);
	m_hbaoFullShader.end();

	CHECK_GL_ERROR();

	m_initialized = true;
}

void HBAO::begin()
{
	CHECK_GL_ERROR();
	m_fboFullRes.begin();
}

void HBAO::endAndRender()
{
	CHECK_GL_ERROR();
	m_fboFullRes.end();
	glDisable(GL_DEPTH_TEST);

	// Apply HBAO //
	m_fboFullRes.bindDepthTexture(GLConfig::HBAO_DEPTH_TEXTURE_BINDING_POINT);
	m_noiseTexture.bind(GLConfig::HBAO_NOISE_TEXTURE_BINDING_POINT);

	m_blurXFbo.begin();
	m_hbaoFullShader.begin();
	m_hbaoGlobalsBuffer.bind();
	QuadDrawer::drawQuad(m_hbaoFullShader);
	m_hbaoFullShader.end();
	m_blurXFbo.end();
	
	// Blur X //
	m_blurXFbo.bindTexture(0, GLConfig::HBAO_DEPTH_TEXTURE_BINDING_POINT);
	m_blurYFbo.begin();
	m_blurXShader.begin();
	QuadDrawer::drawQuad(m_blurXShader);
	m_blurXShader.end();
	m_blurYFbo.end();

	// Blur Y and combine // 
	m_blurYFbo.bindTexture(0, GLConfig::HBAO_DEPTH_TEXTURE_BINDING_POINT);
	m_fboFullRes.bindTexture(0, GLConfig::HBAO_COLOR_TEXTURE_BINDING_POINT);
	m_blurYShader.begin();
	QuadDrawer::drawQuad(m_blurYShader);
	m_blurYShader.end();
	
	glEnable(GL_DEPTH_TEST);	
	CHECK_GL_ERROR();
}