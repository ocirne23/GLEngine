#include "GameScreen.h"

#include "GLEngine.h"
#include "Graphics\Graphics.h"
#include "Input\Input.h"

#include "Graphics\PerspectiveCamera.h"
#include "Graphics\GL\GLSimpleMesh.h"
#include "Graphics\GL\Core\GLShader.h"

#include "Utils\FileHandle.h"
#include "Utils\FPSCameraController.h"
#include "Utils\CheckGLError.h"

#include "rde\vector.h"
#include "rde\rde_string.h"

#include <glm\glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

GLSimpleMesh mesh;
GLShader modelShader;

PerspectiveCamera camera;
FPSCameraController cameraController;

GameScreen::GameScreen(ScreenManager* screenManager) : IScreen(screenManager)
{
	CHECK_GL_ERROR();

	camera.initialize(glm::vec3(0, 0, 0), glm::vec3(0, 0, 1), (float) GLEngine::graphics->getScreenWidth(), (float) GLEngine::graphics->getScreenHeight(), 90.0f, 0.5f, 1500.0f);
	cameraController.initialize(camera, glm::vec3(0, 0, 1));

	modelShader.initialize("Shaders/simplemodelshader.vert", "Shaders/simplemodelshader.frag");
	CHECK_GL_ERROR();

	GLEngine::input->registerKeyListener(&cameraController);
	GLEngine::input->registerMouseListener(&cameraController);
		
	mesh.loadFromFile("Models/crytek-sponza/sponza.da");
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

	//static const glm::mat4 modelMatrix = glm::scale(glm::mat4(1), glm::vec3(0.1f));

	//print("%f %f %f \n", camera.m_position.x, camera.m_position.y, camera.m_position.z);

	modelShader.begin();
	{
		modelShader.setUniformMatrix4f("u_mvp", camera.m_combinedMatrix);
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