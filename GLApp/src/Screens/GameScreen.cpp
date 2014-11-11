#include "GameScreen.h"

#include "..\Core\ScreenManager.h"
#include "GLEngine.h"
#include "Graphics\Graphics.h"
#include "Input\Input.h"

#include "Graphics\PerspectiveCamera.h"

#include "Graphics\GL\GLAppVars.h"

#include "Utils\FileHandle.h"
#include "Utils\FPSCameraController.h"
#include "Utils\CheckGLError.h"
#include "Utils\FileModificationManager.h"

#include "rde\vector.h"
#include "rde\rde_string.h"

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

PerspectiveCamera camera;
FPSCameraController cameraController;

static const unsigned int MAX_LIGHTS = 1024;
static const unsigned int TILE_WIDTH_PX = 32;
static const unsigned int TILE_HEIGHT_PX = 32;
static const unsigned int NUM_TEXTURE_ARRAYS = 16;

GameScreen::GameScreen(ScreenManager* screenManager) : IScreen(screenManager)
{
	CHECK_GL_ERROR();

	GLEngine::input->registerKeyListener(&cameraController);
	GLEngine::input->registerMouseListener(&cameraController);
	GLEngine::input->registerKeyListener(this);
	GLEngine::input->setMouseCaptured(true);

	camera.initialize(glm::vec3(0, 0, 0), glm::vec3(0, 0, 1), (float) GLEngine::graphics->getScreenWidth(), (float) GLEngine::graphics->getScreenHeight(), 90.0f, 0.5f, 1500.0f);
	cameraController.initialize(camera, glm::vec3(0, 0, 1));

	lightManager.initialize(MAX_LIGHTS);
	clusteredShading.initialize(TILE_WIDTH_PX, TILE_HEIGHT_PX, GLEngine::graphics->getViewport(), camera);

	rde::vector<rde::string> extensions;
	rde::vector<rde::string> defines;

	if (rde::string(GLEngine::graphics->getVendorStr()).find("NVIDIA") != rde::string::npos
		&& GLEngine::graphics->getGLMajorVersion() >= 4
		&& GLEngine::graphics->getGLMinorVersion() >= 2)
	{
		defines.push_back(rde::string("DYNAMIC_INDEXING"));
	}

	defines.push_back(rde::string("MAX_LIGHTS ").append(rde::to_string(MAX_LIGHTS)));
	defines.push_back(rde::string("LIGHT_GRID_WIDTH ").append(rde::to_string(clusteredShading.getGridWidth())));
	defines.push_back(rde::string("LIGHT_GRID_HEIGHT ").append(rde::to_string(clusteredShading.getGridHeight())));
	defines.push_back(rde::string("LIGHT_GRID_DEPTH ").append(rde::to_string(clusteredShading.getGridDepth())));
	defines.push_back(rde::string("LIGHT_GRID_TILE_WIDTH ").append(rde::to_string(TILE_WIDTH_PX)));
	defines.push_back(rde::string("LIGHT_GRID_TILE_HEIGHT ").append(rde::to_string(TILE_HEIGHT_PX)));

#ifdef ANDROID
	rde::string versionStr("300 es");
	defines.push_back(rde::string("GL_ES"));
#else
	rde::string versionStr("330 core");
#endif
	modelShader.initialize("Shaders/modelshader.vert", "Shaders/modelshader.frag", versionStr, &defines, &extensions);
	CHECK_GL_ERROR();

	modelShader.begin();
	lightManager.setupShader(modelShader);
	clusteredShading.setupShader(modelShader, 
		GLAppVars::TextureUnits_CLUSTERED_LIGHTING_GRID_TEXTURE, 
		GLAppVars::TextureUnits_CLUSTERED_LIGHTING_LIGHT_ID_TEXTURE);
	modelShader.setUniform3f("u_ambient", glm::vec3(0.2f));
	modelShader.end();
		
	modelShader.begin();
	modelShader.setUniform1i("u_dfvTexture", GLAppVars::TextureUnits_DFV_TEXTURE);
	modelShader.setUniform1i("u_1cTextureArray", GLAppVars::TextureUnits_MODEL_1_COMPONENT_TEXTURE_ARRAY);
	modelShader.setUniform1i("u_3cTextureArray", GLAppVars::TextureUnits_MODEL_3_COMPONENT_TEXTURE_ARRAY);
	mesh.loadFromFile("Models/palace/palace.da", modelShader, 
		GLAppVars::TextureUnits_MODEL_1_COMPONENT_TEXTURE_ARRAY,
		GLAppVars::TextureUnits_MODEL_3_COMPONENT_TEXTURE_ARRAY,
		GLAppVars::UBOBindingPoints_MODEL_MATERIAL_UBO_BINDING_POINT);

	CHECK_GL_ERROR();
	modelShader.end();

	dfvTexture.initialize("Utils/ggx-helper-dfv.da", GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
}

GameScreen::~GameScreen()
{

}

void GameScreen::render(float deltaSec)
{
	GLEngine::graphics->clear(glm::vec4(0.4f, 0.7f, 1.0f, 1.0f));

	cameraController.update(deltaSec);
	camera.update();

	static const glm::mat4 modelMatrix = glm::scale(glm::mat4(1), glm::vec3(1.0));

	dfvTexture.bind(GLAppVars::TextureUnits_DFV_TEXTURE);
	modelShader.begin();
	{
		lightManager.update(camera);
		clusteredShading.update(camera, lightManager.getViewspaceLightPositionRangeListBegin(), lightManager.getNumLights());

		modelShader.setUniform3f("u_eyePos", glm::vec3(camera.m_viewMatrix * glm::vec4(camera.m_position, 1.0f)));
		modelShader.setUniformMatrix4f("u_mv", camera.m_viewMatrix);
		modelShader.setUniformMatrix4f("u_mvp", camera.m_combinedMatrix);
		modelShader.setUniformMatrix3f("u_normalMat", glm::mat3(glm::inverse(glm::transpose(camera.m_viewMatrix))));

		modelShader.setUniformMatrix4f("u_transform", modelMatrix);
		mesh.render();
	}
	modelShader.end();
	FileModificationManager::update();
	CHECK_GL_ERROR();
	
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

bool GameScreen::keyDown(Key key)
{
	switch (key)
	{
	case Key_T:
		{
			lightManager.createLight(
				camera.m_position,
				glm::normalize(glm::vec3(
				(rand() % 1000) / 1000.0f,
				(rand() % 1000) / 1000.0f,
				(rand() % 1000) / 1000.0f)),
				25.0f);
			return true;
		}
	case Key_ESCAPE:
		{
			m_screenManager->quit();
			return true;
		}
	}
	return false;
}
