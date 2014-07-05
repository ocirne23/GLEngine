#include "GameScreen.h"

#include "GLEngine.h"
#include "Graphics\Graphics.h"
#include "Input\Input.h"

#include "Graphics\PerspectiveCamera.h"
#include "Graphics\GL\GLLightManager.h"
#include "Graphics\GL\GLMesh.h"

#include "Graphics\GL\Core\GLVertexBuffer.h"
#include "Graphics\GL\Core\GLStateBuffer.h"
#include "Graphics\GL\Core\GLShader.h"
#include "Graphics\GL\Core\GLTexture.h"

#include "Utils\FileHandle.h"
#include "Utils\FPSCameraController.h"
#include "Utils\CheckGLError.h"

#include "rde\vector.h"
#include "rde\rde_string.h"

#include <glm\glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

static const unsigned int MAX_LIGHTS = 1024;
static const unsigned int TILE_WIDTH_PX = 32;
static const unsigned int TILE_HEIGHT_PX = 32;
static const unsigned int NUM_TEXTURE_ARRAYS = 16;

static const unsigned int MESH_MATERIAL_UBO_BINDING = 0;
static const unsigned int MESH_TEXTURE_INDEX_OFFSET = 3;

GLMesh mesh;
GLLightManager lightManager;
GLShader modelShader;
GLShader testShader;
GLStateBuffer stateBuffer;
GLTexture texture;
GLVertexBuffer buffer;

PerspectiveCamera camera;
FPSCameraController cameraController;

GameScreen::GameScreen(ScreenManager* screenManager) : IScreen(screenManager)
{
	CHECK_GL_ERROR();

	camera.initialize(glm::vec3(0, 0, 0), glm::vec3(0, 0, 1), (float) GLEngine::graphics->getScreenWidth(), (float) GLEngine::graphics->getScreenHeight(), 90.0f, 0.5f, 1500.0f);
	cameraController.initialize(camera, glm::vec3(0, 0, 1));

	testShader.initialize("Shaders/quad.vert", "Shaders/quad.frag");
	modelShader.initialize("Shaders/modelshader.vert", "Shaders/modelshadersimple.frag");
	CHECK_GL_ERROR();

	GLEngine::input->registerKeyListener(&cameraController);
	GLEngine::input->registerMouseListener(&cameraController);
		
	modelShader.begin();
	mesh.loadFromFile("Models/crytek-sponza/sponza.da", false, modelShader, MESH_MATERIAL_UBO_BINDING, MESH_TEXTURE_INDEX_OFFSET);
	modelShader.end();
	CHECK_GL_ERROR();

	//GLEngine::graphics->getTextureManager().initializeTextureBinders();
	CHECK_GL_ERROR();
}

GameScreen::~GameScreen()
{

}

void GameScreen::render(float deltaSec)
{
	GLEngine::graphics->clear(glm::vec4(0.4f, 0.7f, 1.0f, 1.0f));


	
	cameraController.update(deltaSec);
	camera.update();

	static const glm::mat4 modelMatrix = glm::scale(glm::mat4(1), glm::vec3(0.1f));
	
	modelShader.begin();
	{
		modelShader.setUniformMatrix4f("u_mvp", camera.m_combinedMatrix);
		modelShader.setUniformMatrix4f("u_transform", modelMatrix);
		mesh.render();
	}
	modelShader.end();
	

	GLEngine::graphics->swap();
}

void GameScreen::show(uint width, uint height)
{

}

void GameScreen::resize(uint width, uint height)
{

}

void GameScreen::hide()
{

}