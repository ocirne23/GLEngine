#include "HBAO/HBAOTest.h"

#include "GLEngine.h"
#include "Graphics/Graphics.h"
#include "Graphics/PerspectiveCamera.h"
#include "Graphics/Pixmap.h"
#include "Graphics/GL/GL.h"
#include "Graphics/GL/GLMesh.h"
#include "Graphics/GL/GLVars.h"
#include "Graphics/GL/Wrappers/GLTexture.h"
#include "Graphics/GL/Wrappers/GLFramebuffer.h"
#include "Graphics/GL/Wrappers/GLShader.h"
#include "Graphics/GL/Utils/QuadDrawUtils.h"
#include "Input/Input.h"

#include <glm/glm.hpp>
#include <glm/gtx/random.hpp>

//wip 
//https://github.com/scanberg/hbao

BEGIN_UNNAMED_NAMESPACE()

static const char* const MODEL_FILE_PATH = "Models/palace/palace.obj.da";
static const char* const MODEL2_FILE_PATH = "Models/meshes/dragon.obj.da";

static const char* const SKYBOX_FILE_PATH = "Models/skybox/skysphere.obj.da";
static const char* const UI_JSON_FILE_PATH = "UI/uitest.json";

END_UNNAMED_NAMESPACE()

BEGIN_NAMESPACE(UBOBindingPoints)
enum
{
	MODEL_MATERIAL_UBO_BINDING_POINT = 0,
	LIGHT_POSITION_RANGE_UBO_BINDING_POINT,
	LIGHT_COLOR_UBO_BINDING_POINT,
	NUM_ENGINE_RESERVED_UBO_BINDING_POINTS
};
END_NAMESPACE(UBOBindingPoints)

BEGIN_NAMESPACE(TextureUnits)
enum
{
	DFV_TEXTURE = 0,
	CLUSTERED_LIGHTING_GRID_TEXTURE,
	CLUSTERED_LIGHTING_LIGHT_ID_TEXTURE,
	MODEL_TEXTURE_ARRAY,
	NUM_ENGINE_RESERVED_TEXTURE_UNITS
};
END_NAMESPACE(TextureUnits)

#define WIDTH 1280
#define HEIGHT 720

#define RES_RATIO 1
#define AO_WIDTH (WIDTH/RES_RATIO)
#define AO_HEIGHT (HEIGHT/RES_RATIO)
#define AO_RADIUS 1.5
#define AO_DIRS 6
#define AO_SAMPLES 6
#define AO_STRENGTH 1.5;
#define AO_MAX_RADIUS_PIXELS 25.0

#define NOISE_RES 64

void HBAOTest::init(PerspectiveCamera* a_camera)
{
	const uint screenWidth = GLEngine::graphics->getViewportWidth();
	const uint screenHeight = GLEngine::graphics->getViewportHeight();

	m_camera = a_camera;
	
	m_keyDownListener.setFunc([this](EKey a_key) { keyDown(a_key); });
	
	m_building = new GLMesh();
	m_building->loadFromFile(MODEL_FILE_PATH, TextureUnits::MODEL_TEXTURE_ARRAY, UBOBindingPoints::MODEL_MATERIAL_UBO_BINDING_POINT);

	m_dragon = new GLMesh();
	m_dragon->loadFromFile(MODEL2_FILE_PATH, TextureUnits::MODEL_TEXTURE_ARRAY, UBOBindingPoints::MODEL_MATERIAL_UBO_BINDING_POINT);

	//m_skybox = new GLMesh();
	//m_skybox->loadFromFile(SKYBOX_FILE_PATH, TextureUnits::MODEL_TEXTURE_ARRAY, UBOBindingPoints::MODEL_MATERIAL_UBO_BINDING_POINT);

	m_geometryShader = new GLShader();
	m_geometryShader->initialize("Shaders/modelshader.vert", "Shaders/modelshadersimple.frag");

	m_geometryShader->begin();
	m_geometryShader->setUniform1i("u_textureArray", TextureUnits::MODEL_TEXTURE_ARRAY);
	m_geometryShader->setUniform3f("u_ambient", glm::vec3(0.05f));
	m_geometryShader->end();

	m_hbaoFullShader = new GLShader();
	m_hbaoFullShader->initialize("Shaders/HBAO/fullscreen_vert.glsl", "Shaders/HBAO/hbao_full_frag.glsl");

	m_blurXShader = new GLShader();
	m_blurXShader->initialize("Shaders/HBAO/fullscreen_vert.glsl", "Shaders/HBAO/blur_x_frag.glsl");

	m_blurYShader = new GLShader();
	m_blurYShader->initialize("Shaders/HBAO/fullscreen_vert.glsl", "Shaders/HBAO/blur_y_frag.glsl");

	m_fboFullRes = new GLFramebuffer();
	m_fboFullRes->setDepthbufferTexture(GLFramebuffer::ESizedFormat::DEPTH24, screenWidth, screenHeight);
	m_fboFullRes->addFramebufferTexture(GLFramebuffer::ESizedFormat::RGBA8, GLFramebuffer::EAttachment::COLOR0, screenWidth, screenHeight);

	m_blurXFbo = new GLFramebuffer();
	m_blurXFbo->addFramebufferTexture(GLFramebuffer::ESizedFormat::RG16F, GLFramebuffer::EAttachment::COLOR0, screenWidth, screenHeight);
	
	m_blurYFbo = new GLFramebuffer();
	m_blurYFbo->addFramebufferTexture(GLFramebuffer::ESizedFormat::RG16F, GLFramebuffer::EAttachment::COLOR0, screenWidth, screenHeight);

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
	m_noiseTexture = new GLTexture();
	m_noiseTexture->initialize(pixmap, 
							   GLTexture::ETextureMinFilter::NEAREST, GLTexture::ETextureMagFilter::NEAREST,
							   GLTexture::ETextureWrap::REPEAT, GLTexture::ETextureWrap::REPEAT);
	
	
	float fovRad = m_camera->getVFov() * 3.14159265f / 180.0f;
	glm::vec2 FocalLen, InvFocalLen, UVToViewA, UVToViewB, LinMAD, AORes, InvAORes, FullRes, InvFullRes;

	FocalLen[0] = 1.0f / tanf(fovRad * 0.5f) * ((float)AO_HEIGHT / (float)AO_WIDTH);
	FocalLen[1] = 1.0f / tanf(fovRad * 0.5f);
	InvFocalLen[0] = 1.0f / FocalLen[0];
	InvFocalLen[1] = 1.0f / FocalLen[1];

	UVToViewA[0] = -2.0f * InvFocalLen[0];
	UVToViewA[1] = -2.0f * InvFocalLen[1];
	UVToViewB[0] = 1.0f * InvFocalLen[0];
	UVToViewB[1] = 1.0f * InvFocalLen[1];

	float near = m_camera->getNear(), far = m_camera->getFar();
	LinMAD[0] = (near - far) / (2.0f*near*far);
	LinMAD[1] = (near + far) / (2.0f*near*far);

	FullRes[0] = WIDTH;
	FullRes[1] = HEIGHT;

	InvFullRes[0] = 1.0f / WIDTH;
	InvFullRes[1] = 1.0f / HEIGHT;

	AORes[0] = AO_WIDTH;
	AORes[1] = AO_HEIGHT;

	InvAORes[0] = 1.0f / AO_WIDTH;
	InvAORes[1] = 1.0f / AO_HEIGHT;

	float AOStrength = AO_STRENGTH;
	float R = (float) AO_RADIUS;
	float R2 = R * R;
	float NegInvR2 = -1.0f / R2;
	float MaxRadiusPixels = AO_MAX_RADIUS_PIXELS;

	glm::vec2 NoiseScale((float) AO_WIDTH / (float) NOISE_RES, (float) AO_HEIGHT / (float) NOISE_RES);
	int NumDirections = AO_DIRS;
	int NumSamples = AO_SAMPLES;

	m_hbaoFullShader->begin();
	m_hbaoFullShader->setUniform1i("texture0", 0);
	m_hbaoFullShader->setUniform1i("texture1", 1);

	m_hbaoFullShader->setUniform2f("FocalLen", FocalLen);
	m_hbaoFullShader->setUniform2f("UVToViewA", UVToViewA);
	m_hbaoFullShader->setUniform2f("UVToViewB", UVToViewB);
	m_hbaoFullShader->setUniform2f("LinMAD", LinMAD);

	m_hbaoFullShader->setUniform2f("AORes", AORes);
	m_hbaoFullShader->setUniform2f("InvAORes", InvAORes);

	m_hbaoFullShader->setUniform1f("AOStrength", AOStrength);
	m_hbaoFullShader->setUniform1f("R", R);
	m_hbaoFullShader->setUniform1f("R2", R2);
	m_hbaoFullShader->setUniform1f("NegInvR2", NegInvR2);
	m_hbaoFullShader->setUniform1f("MaxRadiusPixels", MaxRadiusPixels);

	m_hbaoFullShader->setUniform2f("NoiseScale", NoiseScale);
	m_hbaoFullShader->setUniform1i("NumDirections", NumDirections);
	m_hbaoFullShader->setUniform1i("NumSamples", NumSamples);
	m_hbaoFullShader->end();

	m_blurXShader->begin();
	m_blurXShader->setUniform1i("texture0", 0);

	m_blurXShader->setUniform2f("AORes", AORes);
	m_blurXShader->setUniform2f("InvAORes", InvAORes);
	m_blurXShader->setUniform2f("FullRes", FullRes);
	m_blurXShader->setUniform2f("InvFullRes", InvFullRes);
	m_blurXShader->setUniform2f("LinMAD", LinMAD);
	m_blurXShader->end();

	m_blurYShader->begin();
	m_blurYShader->setUniform1i("texture0", 0);
	m_blurYShader->setUniform1i("texture1", 1);

	m_blurYShader->setUniform2f("AORes", AORes);
	m_blurYShader->setUniform2f("InvAORes", InvAORes);
	m_blurYShader->setUniform2f("FullRes", FullRes);
	m_blurYShader->setUniform2f("InvFullRes", InvFullRes);
	m_blurYShader->setUniform2f("LinMAD", LinMAD);
	m_blurYShader->end();
}

void HBAOTest::render(float a_deltaSec)
{
	// RENDER GEOMETRY PASS //
	glEnable(GL_DEPTH_TEST);
	m_fboFullRes->begin();
	GLEngine::graphics->clear(glm::vec4(0.2f, 0.3f, 0.5f, 1.0f));

	m_geometryShader->begin();
	m_geometryShader->setUniformMatrix4f("u_viewMatrix", m_camera->getViewMatrix());
	m_geometryShader->setUniformMatrix4f("u_viewProjectionMatrix", m_camera->getCombinedMatrix());
	m_geometryShader->setUniformMatrix3f("u_normalMatrix", glm::mat3(glm::inverse(glm::transpose(m_camera->getViewMatrix()))));
	glm::mat4 transform(1);

	transform[3] = glm::vec4(0.0f, -10.0f, -70.0f, 1.0f);
	m_geometryShader->setUniformMatrix4f("u_modelMatrix", transform);
	m_building->render(*m_geometryShader);

	transform[3] = glm::vec4(0.0f, -7.0f, -58.0f, 1.0f);
	m_geometryShader->setUniformMatrix4f("u_modelMatrix", transform);
	m_dragon->render(*m_geometryShader);

	m_geometryShader->end();
	m_fboFullRes->end();

	glDisable(GL_DEPTH_TEST);

	// Apply HBAO //
	m_fboFullRes->bindDepthTexture(0);
	m_noiseTexture->bind(1);

	m_blurXFbo->begin();
	m_hbaoFullShader->begin();
	QuadDrawUtils::drawQuad(*m_hbaoFullShader);
	m_hbaoFullShader->end();
	m_blurXFbo->end();

	// Blur X //
	m_blurXFbo->bindTexture(0, 0);
	m_blurYFbo->begin();
	m_blurXShader->begin();
	QuadDrawUtils::drawQuad(*m_blurXShader);
	m_blurXShader->end();
	m_blurYFbo->end();

	// Blur Y and combine // 
	m_blurYFbo->bindTexture(0, 0);
	m_fboFullRes->bindTexture(0, 1);
	m_blurYShader->begin();
	QuadDrawUtils::drawQuad(*m_blurYShader);
	m_blurYShader->end();
	
	GLEngine::graphics->swap();
}

void HBAOTest::keyDown(EKey a_key)
{
	switch (a_key)
	{
	case EKey::ESCAPE:
		GLEngine::shutdown();
		break;
	}
}