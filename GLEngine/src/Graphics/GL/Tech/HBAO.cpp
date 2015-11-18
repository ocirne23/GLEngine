#include "Graphics/GL/Tech/HBAO.h"

#include "GLEngine.h"
#include "Graphics/Graphics.h"
#include "Graphics/PerspectiveCamera.h"
#include "Graphics/Pixmap.h"
#include "Graphics/GL/GL.h"
#include "Graphics/GL/Utils/QuadDrawUtils.h"
#include "Graphics/GL/Utils/CheckGLError.h"

#include <glm/glm.hpp>
#include <glm/gtx/random.hpp>

BEGIN_UNNAMED_NAMESPACE()

static const float RES_RATIO = 1.0f;
static const float AO_RADIUS = 1.5f;
static const uint AO_DIRS = 6;
static const uint AO_SAMPLES = 6;
static const float AO_STRENGTH = 1.5f;
static const float AO_ANGLE_BIAS_DEG = 30.0f;
static const uint NOISE_RES = 8;

struct HBAOGlobals
{
	glm::vec2 fullResolution;
	glm::vec2 invFullResolution;

	glm::vec2 aoResolution;
	glm::vec2 invAOResolution;

	glm::vec2 focalLen;
	glm::vec2 invFocalLen;

	glm::vec2 uvToViewA;
	glm::vec2 uvToViewB;

	float r;
	float r2;
	float negInvR2;
	float maxRadiusPixels;

	float angleBias;
	float tanAngleBias;
	float strength;
	float dummy;

	glm::vec2 noiseTexScale;
	glm::vec2 ndcDepthConv;
};

END_UNNAMED_NAMESPACE()

HBAO::~HBAO()
{

}

void HBAO::initialize(const PerspectiveCamera& a_camera, uint a_xRes, uint a_yRes, uint a_hbaoGlobalsUBOBindingPoint)
{
	const uint screenWidth = a_xRes;
	const uint screenHeight = a_yRes;
	const float aoWidth = screenWidth / RES_RATIO;
	const float aoHeight = screenHeight / RES_RATIO;

	m_hbaoFullShader.initialize("Shaders/quad.vert", "Shaders/HBAO/HBAO.frag");
	m_blurXShader.initialize("Shaders/quad.vert", "Shaders/HBAO/blurx.frag");
	m_blurYShader.initialize("Shaders/quad.vert", "Shaders/HBAO/blury.frag");
	m_fboFullRes.initialize(GLFramebuffer::EMultiSampleType::NONE);
	m_blurXFbo.initialize();
	m_blurYFbo.initialize();

	m_fboFullRes.setDepthbufferTexture(GLFramebuffer::ESizedFormat::DEPTH24, screenWidth, screenHeight);
	m_fboFullRes.addFramebufferTexture(GLFramebuffer::ESizedFormat::RGB8, GLFramebuffer::EAttachment::COLOR0, screenWidth, screenHeight);
	m_blurXFbo.addFramebufferTexture(GLFramebuffer::ESizedFormat::RG16F, GLFramebuffer::EAttachment::COLOR0, screenWidth, screenHeight);
	m_blurYFbo.addFramebufferTexture(GLFramebuffer::ESizedFormat::RG16F, GLFramebuffer::EAttachment::COLOR0, screenWidth, screenHeight);
	CHECK_GL_ERROR();

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
	Pixmap pixmap;
	pixmap.set(noiseTexWidth, noiseTexHeight, 4, true, noise);
	
	m_noiseTexture.initialize(pixmap,
		GLTexture::ETextureMinFilter::NEAREST, GLTexture::ETextureMagFilter::NEAREST,
		GLTexture::ETextureWrap::REPEAT, GLTexture::ETextureWrap::REPEAT);

	float fovRad = glm::radians(a_camera.getVFov());
	float near = a_camera.getNear();
	float far = a_camera.getFar();

	HBAOGlobals globals;
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
	globals.dummy             = 0.0f;
	globals.noiseTexScale     = glm::vec2(screenWidth / (float) noiseTexWidth, screenHeight / (float) noiseTexHeight);
	globals.ndcDepthConv.x    = (near - far) / (2.0f * near * far);
	globals.ndcDepthConv.y    = (near + far) / (2.0f * near * far);

	m_hbaoFullShader.begin();
	m_hbaoFullShader.setUniform1i("u_linearDepthTex", 0);
	m_hbaoFullShader.setUniform1i("u_randomTex", 1);
	m_hbaoFullShader.end();

	m_blurXShader.begin();
	m_blurXShader.setUniform1i("u_aoDepthTex", 0);
	m_blurXShader.setUniform2f("u_invFullRes", globals.invFullResolution);
	m_blurXShader.end();

	m_blurYShader.begin();
	m_blurYShader.setUniform1i("u_aoDepthTex", 0);
	m_blurYShader.setUniform1i("u_colorTex", 1);
	m_blurYShader.setUniform2f("u_invFullRes", globals.invFullResolution);
	m_blurYShader.end();

	m_hbaoFullShader.begin();
	m_hbaoGlobalsBuffer.initialize(m_hbaoFullShader, a_hbaoGlobalsUBOBindingPoint, "HBAOGlobals", GLConstantBuffer::EDrawUsage::STATIC);
	m_hbaoGlobalsBuffer.upload(sizeof(HBAOGlobals), &globals);
	m_hbaoFullShader.end();

	m_initialized = true;
}

void HBAO::begin()
{
	m_fboFullRes.begin();
	CHECK_GL_ERROR();
}

void HBAO::endAndRender()
{
	CHECK_GL_ERROR();

	m_fboFullRes.end();
	glDisable(GL_DEPTH_TEST);

	// Apply HBAO //
	m_fboFullRes.bindDepthTexture(0);
	m_noiseTexture.bind(1);

	m_blurXFbo.begin();
	m_hbaoFullShader.begin();
	m_hbaoGlobalsBuffer.bind();
	QuadDrawUtils::drawQuad(m_hbaoFullShader);
	m_hbaoFullShader.end();
	m_blurXFbo.end();
	
	// Blur X //
	m_blurXFbo.bindTexture(0, 0);
	m_blurYFbo.begin();
	m_blurXShader.begin();
	QuadDrawUtils::drawQuad(m_blurXShader);
	m_blurXShader.end();
	m_blurYFbo.end();

	// Blur Y and combine // 
	m_blurYFbo.bindTexture(0, 0);
	m_fboFullRes.bindTexture(0, 1);
	m_blurYShader.begin();
	QuadDrawUtils::drawQuad(m_blurYShader);
	m_blurYShader.end();
	glEnable(GL_DEPTH_TEST);	
	CHECK_GL_ERROR();
}