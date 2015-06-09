#include "Graphics/GL/Tech/HBAO.h"

#include "GLEngine.h"
#include "Graphics/Graphics.h"
#include "Graphics/PerspectiveCamera.h"
#include "Graphics/Pixmap.h"
#include "Graphics/GL/GL.h"
#include "Graphics/GL/Utils/QuadDrawUtils.h"

#include <glm/glm.hpp>
#include <glm/gtx/random.hpp>

BEGIN_UNNAMED_NAMESPACE()

static const float RES_RATIO = 1.0f;
static const float AO_RADIUS = 1.0f;
static const uint AO_DIRS = 6;
static const uint AO_SAMPLES = 6;
static const float AO_STRENGTH = 1.5f;
static const float AO_MAX_RADIUS_PIXELS = 25.0f;
static const uint NOISE_RES = 64;

END_UNNAMED_NAMESPACE()

HBAO::~HBAO()
{

}

void HBAO::initialize(const PerspectiveCamera& a_camera)
{
	const uint screenWidth = GLEngine::graphics->getViewportWidth();
	const uint screenHeight = GLEngine::graphics->getViewportHeight();
	const float aoWidth = screenWidth / RES_RATIO;
	const float aoHeight = screenHeight / RES_RATIO;

	m_hbaoFullShader.initialize("Shaders/HBAO/fullscreen_vert.glsl", "Shaders/HBAO/hbao_full_frag.glsl");
	m_blurXShader.initialize("Shaders/HBAO/fullscreen_vert.glsl", "Shaders/HBAO/blur_x_frag.glsl");
	m_blurYShader.initialize("Shaders/HBAO/fullscreen_vert.glsl", "Shaders/HBAO/blur_y_frag.glsl");

	m_fboFullRes.setDepthbufferTexture(GLFramebuffer::ESizedFormat::DEPTH24, screenWidth, screenHeight);
	m_fboFullRes.addFramebufferTexture(GLFramebuffer::ESizedFormat::RGB8, GLFramebuffer::EAttachment::COLOR0, screenWidth, screenHeight);
	m_blurXFbo.addFramebufferTexture(GLFramebuffer::ESizedFormat::RG16F, GLFramebuffer::EAttachment::COLOR0, screenWidth, screenHeight);
	m_blurYFbo.addFramebufferTexture(GLFramebuffer::ESizedFormat::RG16F, GLFramebuffer::EAttachment::COLOR0, screenWidth, screenHeight);

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

			int offset = 4 * (y*noiseTexWidth + x);
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

	float fovRad = a_camera.getVFov() * 3.14159265f / 180.0f;
	glm::vec2 FocalLen, InvFocalLen, UVToViewA, UVToViewB, LinMAD, AORes, InvAORes, FullRes, InvFullRes;

	FocalLen[0] = 1.0f / tanf(fovRad * 0.5f) * ((float) aoHeight / (float) aoWidth);
	FocalLen[1] = 1.0f / tanf(fovRad * 0.5f);
	InvFocalLen[0] = 1.0f / FocalLen[0];
	InvFocalLen[1] = 1.0f / FocalLen[1];

	UVToViewA[0] = -2.0f * InvFocalLen[0];
	UVToViewA[1] = -2.0f * InvFocalLen[1];
	UVToViewB[0] = 1.0f * InvFocalLen[0];
	UVToViewB[1] = 1.0f * InvFocalLen[1];

	float near = a_camera.getNear(), far = a_camera.getFar();
	LinMAD[0] = (near - far) / (2.0f*near*far);
	LinMAD[1] = (near + far) / (2.0f*near*far);

	FullRes[0] = (float) screenWidth;
	FullRes[1] = (float) screenHeight;

	InvFullRes[0] = 1.0f / screenWidth;
	InvFullRes[1] = 1.0f / screenHeight;

	AORes[0] = aoWidth;
	AORes[1] = aoHeight;

	InvAORes[0] = 1.0f / aoWidth;
	InvAORes[1] = 1.0f / aoHeight;

	float AOStrength = AO_STRENGTH;
	float R = (float)AO_RADIUS;
	float R2 = R * R;
	float NegInvR2 = -1.0f / R2;
	float MaxRadiusPixels = AO_MAX_RADIUS_PIXELS;

	glm::vec2 NoiseScale((float) aoWidth / (float) NOISE_RES, (float)aoHeight / (float) NOISE_RES);
	int NumDirections = AO_DIRS;
	int NumSamples = AO_SAMPLES;

	m_hbaoFullShader.begin();
	m_hbaoFullShader.setUniform1i("texture0", 0);
	m_hbaoFullShader.setUniform1i("texture1", 1);
	m_hbaoFullShader.setUniform2f("FocalLen", FocalLen);
	m_hbaoFullShader.setUniform2f("UVToViewA", UVToViewA);
	m_hbaoFullShader.setUniform2f("UVToViewB", UVToViewB);
	m_hbaoFullShader.setUniform2f("LinMAD", LinMAD);
	m_hbaoFullShader.setUniform2f("AORes", AORes);
	m_hbaoFullShader.setUniform2f("InvAORes", InvAORes);
	m_hbaoFullShader.setUniform1f("AOStrength", AOStrength);
	m_hbaoFullShader.setUniform1f("R", R);
	m_hbaoFullShader.setUniform1f("R2", R2);
	m_hbaoFullShader.setUniform1f("NegInvR2", NegInvR2);
	m_hbaoFullShader.setUniform1f("MaxRadiusPixels", MaxRadiusPixels);
	m_hbaoFullShader.setUniform2f("NoiseScale", NoiseScale);
	m_hbaoFullShader.setUniform1i("NumDirections", NumDirections);
	m_hbaoFullShader.setUniform1i("NumSamples", NumSamples);
	m_hbaoFullShader.end();

	m_blurXShader.begin();
	m_blurXShader.setUniform1i("texture0", 0);
	m_blurXShader.setUniform2f("AORes", AORes);
	m_blurXShader.setUniform2f("InvAORes", InvAORes);
	m_blurXShader.setUniform2f("FullRes", FullRes);
	m_blurXShader.setUniform2f("InvFullRes", InvFullRes);
	m_blurXShader.setUniform2f("LinMAD", LinMAD);
	m_blurXShader.end();

	m_blurYShader.begin();
	m_blurYShader.setUniform1i("texture0", 0);
	m_blurYShader.setUniform1i("texture1", 1);
	m_blurYShader.setUniform2f("AORes", AORes);
	m_blurYShader.setUniform2f("InvAORes", InvAORes);
	m_blurYShader.setUniform2f("FullRes", FullRes);
	m_blurYShader.setUniform2f("InvFullRes", InvFullRes);
	m_blurYShader.setUniform2f("LinMAD", LinMAD);
	m_blurYShader.end();

	m_isInitialized = true;
}

void HBAO::begin()
{
	glEnable(GL_DEPTH_TEST);
	m_fboFullRes.begin();
}

void HBAO::endAndRender()
{
	m_fboFullRes.end();
	glDisable(GL_DEPTH_TEST);

	// Apply HBAO //
	m_fboFullRes.bindDepthTexture(0);
	m_noiseTexture.bind(1);

	m_blurXFbo.begin();
	m_hbaoFullShader.begin();
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
}