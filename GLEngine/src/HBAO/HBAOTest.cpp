#include "HBAO/HBAOTest.h"

#include "GLEngine.h"
#include "Graphics/Graphics.h"
#include "Graphics/PerspectiveCamera.h"
#include "Graphics/Pixmap.h"
#include "Graphics/GL/GLMesh.h"
#include "Graphics/GL/GLVars.h"
#include "Input/Input.h"

//wip 
//https://github.com/scanberg/hbao

BEGIN_UNNAMED_NAMESPACE()

static const char* const MODEL_FILE_PATH = "Models/palace/palace.obj.da";
static const char* const MODEL2_FILE_PATH = "Models/meshes/dragon.obj.da";

static const char* const SKYBOX_FILE_PATH = "Models/skybox/skysphere.obj.da";
static const char* const UI_JSON_FILE_PATH = "UI/uitest.json";

GLuint generateNoiseTexture(int width, int height)
{


	GLint internalFormat = GL_RGBA16F;
	GLint format = GL_RGBA;
	GLint type = GL_FLOAT;

	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, noise);
	return tex;
}

END_UNNAMED_NAMESPACE()

#define WIDTH 1280
#define HEIGHT 720

#define RES_RATIO 2
#define AO_WIDTH (WIDTH/RES_RATIO)
#define AO_HEIGHT (HEIGHT/RES_RATIO)
#define AO_RADIUS 0.3
#define AO_DIRS 6
#define AO_SAMPLES 3
#define AO_STRENGTH 2.5;
#define AO_MAX_RADIUS_PIXELS 50.0

#define NOISE_RES 4

#define MOVE_SPEED 3.5
#define MOUSE_SPEED 9.5

bool blur = false;
bool fullres = true;

void HBAOTest::init(PerspectiveCamera* a_camera)
{
	m_camera = a_camera;

	m_camera = new PerspectiveCamera();
	m_camera->initialize((float) GLEngine::graphics->getViewportWidth(), (float) GLEngine::graphics->getViewportHeight(), 90.0f, 0.5f, 1500.0f);

	m_building = new GLMesh();
	m_building->loadFromFile(MODEL_FILE_PATH, TextureUnits::MODEL_TEXTURE_ARRAY, UBOBindingPoints::MODEL_MATERIAL_UBO_BINDING_POINT);

	m_dragon = new GLMesh();
	m_dragon->loadFromFile(MODEL2_FILE_PATH, TextureUnits::MODEL_TEXTURE_ARRAY, UBOBindingPoints::MODEL_MATERIAL_UBO_BINDING_POINT);

	m_skybox = new GLMesh();
	m_skybox->loadFromFile(SKYBOX_FILE_PATH, TextureUnits::MODEL_TEXTURE_ARRAY, UBOBindingPoints::MODEL_MATERIAL_UBO_BINDING_POINT);

	m_keyDownListener.setFunc([this](EKey a_key) { keyDown(a_key); });

	uint noiseTexWidth = NOISE_RES;
	uint noiseTexHeight = NOISE_RES;
	float *noise = new float[noiseTexWidth*noiseTexHeight * 4];
	for (int y = 0; y < noiseTexHeight; ++y)
	{
		for (int x = 0; x < noiseTexWidth; ++x)
		{
			vec2 xy = glm::circularRand(1.0f);
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
}

void HBAOTest::render(float a_deltaSec)
{
	// RENDER GEOMETRY PASS
	glEnable(GL_DEPTH_TEST);

	fboFullRes->bind();
	glClearColor(0.2, 0.3, 0.5, 1.0);
	glClearDepth(1.0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	geometryShader->bind();

	glUniformMatrix4fv(geometryShader->getViewMatrixLocation(), 1, false, glm::value_ptr(cam->getViewMatrix()));
	glUniformMatrix4fv(geometryShader->getProjMatrixLocation(), 1, false, glm::value_ptr(cam->getProjMatrix()));

	mdl->draw();
	//model->draw();

	glDisable(GL_DEPTH_TEST);

	//glColorMask(1, 0, 0, 0);
	//glDepthFunc(GL_ALWAYS);

	if (!fullres)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, fboFullRes->getBufferHandle(FBO_DEPTH));

		// Downsample depth
		fboHalfRes->bind();

		buffer[0] = GL_COLOR_ATTACHMENT0;
		glDrawBuffers(1, buffer);

		downsampleShader->bind();
		fsquad->draw();

	}

	// AO pass

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, noiseTexture);

	if (!fullres)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, fboHalfRes->getBufferHandle(FBO_AUX0));

		buffer[0] = GL_COLOR_ATTACHMENT1;
		glDrawBuffers(1, buffer);

		hbaoHalfShader->bind();

		fsquad->draw();
	}
	else
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, fboFullRes->getBufferHandle(FBO_DEPTH));

		buffer[0] = GL_COLOR_ATTACHMENT1;
		glDrawBuffers(1, buffer);

		hbaoFullShader->bind();
		fsquad->draw();
	}

	// Upsample

	if (!fullres)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, fboFullRes->getBufferHandle(FBO_DEPTH));

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, fboHalfRes->getBufferHandle(FBO_AUX1));

		fboFullRes->bind();

		buffer[0] = GL_COLOR_ATTACHMENT1;
		glDrawBuffers(1, buffer);

		upsampleShader->bind();

		fsquad->draw();
	}

	//timeStamps[4] = glfwGetTime();
	glQueryCounter(queryID[4], GL_TIMESTAMP);

	if (blur)
	{
		// BLUR 

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, fboFullRes->getBufferHandle(FBO_AUX1));

		// X
		buffer[0] = GL_COLOR_ATTACHMENT2;
		glDrawBuffers(1, buffer);

		blurXShader->bind();

		fsquad->draw();

		// Y
		buffer[0] = GL_COLOR_ATTACHMENT1;
		glDrawBuffers(1, buffer);

		blurYShader->bind();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, fboFullRes->getBufferHandle(FBO_AUX2));

		fsquad->draw();

	}

	//timeStamps[5] = glfwGetTime();
	glQueryCounter(queryID[5], GL_TIMESTAMP);

	fboFullRes->unbind();

	// RENDER TO SCREEN PASS

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, fboFullRes->getBufferHandle(FBO_AUX0));

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, fboFullRes->getBufferHandle(FBO_AUX1));

	//glClearColor(1.0, 1.0, 1.0, 0.0);
	//glClear(GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, WIDTH, HEIGHT);

	compositShader->bind();

	fsquad->draw();

	glfwSwapBuffers();

	if (!glfwGetWindowParam(GLFW_OPENED))
		running = false;

	//timeStamps[6] = glfwGetTime();
	glQueryCounter(queryID[6], GL_TIMESTAMP);

	// wait until the results are available
	GLint stopTimerAvailable = 0;
	bool stop = false;

	while (!stop)
	{
		stop = true;
		for (int i = 0; i < 7; ++i)
		{
			glGetQueryObjectiv(queryID[1], GL_QUERY_RESULT_AVAILABLE, &stopTimerAvailable);
			if (!stopTimerAvailable)
			  stop = false;
		}
	}
}








void HBAOTest::keyDown(EKey a_key)
{
	switch (a_key)
	{
	case EKey::T:
	{
		glm::vec3 position = m_camera->getPosition() + m_camera->getDirection();
		glm::vec3 color = glm::normalize(glm::vec3((rand() % 1000) / 1000.0f, (rand() % 1000) / 1000.0f, (rand() % 1000) / 1000.0f));
		float intensity = 20.0f * ((rand() % 1000) / 1000.0f) + 1.0f;
		float radius = 20.0f * ((rand() % 1000) / 1000.0f) + 1.0f;
		entityx::Entity lightEntity = m_entityx.entities.create();
		lightEntity.assign<PointLightComponent>()->set(position, radius, color, intensity);
		break;
	}
	case EKey::Y:
	{
		for (entityx::Entity e : m_entityx.entities.entities_with_components<PointLightComponent>())
		{
			e.component<PointLightComponent>().remove();
			e.destroy();
		}
		break;
	}
	case EKey::ESCAPE:
	{
		GLEngine::shutdown();
		break;
	}
	}
}