#include "Screens/DBTestScreen.h"

#include "GLEngine.h"
#include "Graphics/Graphics.h"

DBTestScreen::DBTestScreen()
{
	print("DBTestScreen\n");
	m_keyDownListener.setFunc([this](EKey a_key) { keyDown(a_key); });
	m_keyUpListener.setFunc([this](EKey a_key) { m_cameraController.keyUp(a_key); });
	m_windowQuitListener.setFunc([this]() { GLEngine::shutdown(); });
	
	m_mouseMovedListener.setFunc([this](uint xPos, uint yPos, int deltaX, int deltaY) {
		m_cameraController.mouseMoved(xPos, yPos, deltaX, deltaY);
	});

	m_db.openExisting("Assets/db.da");
	m_scene.initialize("palace.obj", m_db);
	m_basicSceneShader.initialize("Shaders/modelshader.vert", "Shaders/modelshadersimple.frag");

	uint viewportWidth = GLEngine::graphics->getViewportWidth();
	uint viewportHeight = GLEngine::graphics->getViewportHeight();
	m_renderer.initialize();

	m_camera.initialize((float) viewportWidth, (float) viewportHeight, 90.0f, 0.5f, 1000.0f);
	m_cameraController.initialize(m_camera, glm::vec3(0, 1, 0));
}

DBTestScreen::~DBTestScreen()
{
}

void DBTestScreen::render(float a_deltaSec)
{
	GLEngine::graphics->clear(glm::vec4(0.8f, 0.8f, 0.8f, 1.0f));

	m_cameraController.update(a_deltaSec);
	m_camera.updateMatrices();
	m_renderer.renderScene(m_scene, m_camera);

	GLEngine::graphics->swap();
}

void DBTestScreen::keyDown(EKey a_key)
{
	m_cameraController.keyDown(a_key);
	switch (a_key)
	{
	case EKey::ESCAPE:
	{
		GLEngine::shutdown();
		break;
	}
	}
}