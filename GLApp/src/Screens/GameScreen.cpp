#include "GameScreen.h"

#include "Core/ScreenManager.h"
#include "GLEngine.h"
#include "Graphics/Graphics.h"
#include "Graphics/GL/GLAppVars.h"
#include "Input/Input.h"
#include "Utils/FileHandle.h"
#include "Utils/CheckGLError.h"
#include "Utils/FileModificationManager.h"
#include "rde/vector.h"
#include "rde/rde_string.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

BEGIN_UNNAMED_NAMESPACE()

static const unsigned int MAX_LIGHTS = 1024;
static const unsigned int TILE_WIDTH_PX = 32;
static const unsigned int TILE_HEIGHT_PX = 32;

END_UNNAMED_NAMESPACE()

GameScreen::GameScreen(ScreenManager* a_screenManager) : IScreen(a_screenManager), m_lightManager(MAX_LIGHTS)
{
	GLEngine::input->registerKeyListener(&m_cameraController);
	GLEngine::input->registerMouseListener(&m_cameraController);
	GLEngine::input->registerKeyListener(this);
	GLEngine::input->setMouseCaptured(true);

	m_camera.initialize(glm::vec3(0, 0, 0), glm::vec3(0, 0, 1), (float) GLEngine::graphics->getScreenWidth(), (float) GLEngine::graphics->getScreenHeight(), 90.0f, 0.5f, 1500.0f);
	m_cameraController.initialize(m_camera, glm::vec3(0, 0, 1));
	m_clusteredShading.initialize(TILE_WIDTH_PX, TILE_HEIGHT_PX, GLEngine::graphics->getViewport(), m_camera);

	rde::vector<rde::string> defines;
	defines.push_back(rde::string("MAX_LIGHTS ").append(rde::to_string(MAX_LIGHTS)));
	defines.push_back(rde::string("LIGHT_GRID_WIDTH ").append(rde::to_string(m_clusteredShading.getGridWidth())));
	defines.push_back(rde::string("LIGHT_GRID_HEIGHT ").append(rde::to_string(m_clusteredShading.getGridHeight())));
	defines.push_back(rde::string("LIGHT_GRID_DEPTH ").append(rde::to_string(m_clusteredShading.getGridDepth())));
	defines.push_back(rde::string("LIGHT_GRID_TILE_WIDTH ").append(rde::to_string(TILE_WIDTH_PX)));
	defines.push_back(rde::string("LIGHT_GRID_TILE_HEIGHT ").append(rde::to_string(TILE_HEIGHT_PX)));

	m_modelShader.initialize("Shaders/modelshader.vert", "Shaders/modelshader.frag", &defines);
	
	m_modelShader.begin();
	m_modelShader.setUniform1i("u_dfvTexture", GLAppVars::TextureUnits_DFV_TEXTURE);
	m_modelShader.setUniform1i("u_1cTextureArray", GLAppVars::TextureUnits_MODEL_1_COMPONENT_TEXTURE_ARRAY);
	m_modelShader.setUniform1i("u_3cTextureArray", GLAppVars::TextureUnits_MODEL_3_COMPONENT_TEXTURE_ARRAY);
	m_modelShader.setUniform1f("u_recLogSD1", m_clusteredShading.getRecLogSD1());
	m_modelShader.setUniform1f("u_recNear", m_clusteredShading.getRecNear());
	m_modelShader.setUniform3f("u_ambient", glm::vec3(0.2f));
	m_modelShader.setUniformMatrix4f("u_transform", glm::mat4(1));

	m_viewMatrixUniform.initialize(m_modelShader, "u_mv");
	m_mvpMatrixUniform.initialize(m_modelShader, "u_mvp");
	m_normalMatrixUniform.initialize(m_modelShader, "u_normalMat");

	m_lightPositionRangeBuffer.initialize(m_modelShader, GLAppVars::UBOBindingPoints_LIGHT_POSITION_RANGE_UBO_BINDING_POINT, "LightPositionRanges", GL_STREAM_DRAW);
	m_lightColorBuffer.initialize(m_modelShader, GLAppVars::UBOBindingPoints_LIGHT_COLOR_UBO_BINDING_POINT, "LightColors", GL_STREAM_DRAW);
	
	m_lightGridTextureBuffer.initialize(m_modelShader, "u_lightGrid", GLAppVars::TextureUnits_CLUSTERED_LIGHTING_GRID_TEXTURE, GL_RG32UI, GL_STREAM_DRAW);
	m_lightIndiceTextureBuffer.initialize(m_modelShader, "u_lightIndices", GLAppVars::TextureUnits_CLUSTERED_LIGHTING_LIGHT_ID_TEXTURE, GL_R16UI, GL_STREAM_DRAW);
	m_lightGridTextureBuffer.bind();
	m_lightIndiceTextureBuffer.bind();

	m_mesh.loadFromFile("Models/palace/palace.da", m_modelShader,
		GLAppVars::TextureUnits_MODEL_1_COMPONENT_TEXTURE_ARRAY,
		GLAppVars::TextureUnits_MODEL_3_COMPONENT_TEXTURE_ARRAY,
		GLAppVars::UBOBindingPoints_MODEL_MATERIAL_UBO_BINDING_POINT);

	m_modelShader.end();

	m_dfvTexture.initialize("Utils/ggx-helper-dfv.da", GLAppVars::TextureUnits_DFV_TEXTURE, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	m_dfvTexture.bind();
}

void GameScreen::render(float a_deltaSec)
{
	GLEngine::graphics->clear(glm::vec4(0.4f, 0.7f, 1.0f, 1.0f));

	m_cameraController.update(a_deltaSec);
	m_camera.update();

	m_modelShader.begin();
	{
		const glm::vec4* viewspaceLightPositionRanges = m_lightManager.updateViewspaceLightPositionRangeList(m_camera);
		m_clusteredShading.update(m_camera, m_lightManager.getNumLights(), viewspaceLightPositionRanges);

		m_lightPositionRangeBuffer.upload(m_lightManager.getNumLights() * sizeof(glm::vec4), viewspaceLightPositionRanges);
		m_lightColorBuffer.upload(m_lightManager.getNumLights() * sizeof(glm::vec4), m_lightManager.getLightColors());
		m_lightGridTextureBuffer.upload(m_clusteredShading.getGridSize() * sizeof(glm::uvec2), m_clusteredShading.getLightGrid());
		m_lightIndiceTextureBuffer.upload(m_clusteredShading.getNumLightIndices() * sizeof(ushort), m_clusteredShading.getLightIndices());

		m_mvpMatrixUniform.set(m_camera.m_combinedMatrix);
		m_viewMatrixUniform.set(m_camera.m_viewMatrix);
		m_normalMatrixUniform.set(glm::mat3(glm::inverse(glm::transpose(m_camera.m_viewMatrix))));

		m_mesh.render();
	}
	m_modelShader.end();
	FileModificationManager::update();
	
	GLEngine::graphics->swap();
}

rde::vector<LightHandle> lights;

bool GameScreen::keyDown(Key a_key)
{
	switch (a_key)
	{
	case Key_T:
	{
		LightHandle h = m_lightManager.createLight(
			m_camera.m_position,
			glm::normalize(glm::vec3(
			(rand() % 1000) / 1000.0f,
			(rand() % 1000) / 1000.0f,
			(rand() % 1000) / 1000.0f)),
			5.0f);
		lights.push_back(h);
		return true;
	}

	case Key_Y:
	{
		if (lights.size())
		{
			LightHandle h = lights.back();
			m_lightManager.deleteLight(h);
			lights.pop_back();
		}
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

GameScreen::~GameScreen() {}
void GameScreen::show(uint a_width, uint a_height) {}
void GameScreen::resize(uint a_width, uint a_height) {}
void GameScreen::hide() {}