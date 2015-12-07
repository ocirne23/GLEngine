#include "Screens/DBTestScreen.h"

#include "GLEngine.h"
#include "Graphics/Graphics.h"
#include "Graphics/GL/Scene/GLConfig.h"

#include <glm/gtc/random.hpp>

DBTestScreen::DBTestScreen() : m_lightManager(GLConfig::MAX_LIGHTS)
{
	uint viewportWidth = GLEngine::graphics->getViewportWidth();
	uint viewportHeight = GLEngine::graphics->getViewportHeight();
	m_camera.initialize((float) viewportWidth, (float) viewportHeight, 90.0f, 0.1f, 1000.0f);
	m_camera.setPosition(4.0f, 1.0f, 4.0f);
	m_camera.lookAtPoint(-4.0f, 6.0f, -4.0f);
	m_cameraController.initialize(m_camera);

	m_ifcDB.openExisting("Assets/IFC-DB.da");
	m_ifcScene.initialize("ifc2.ifc", m_ifcDB);
	m_renderer.addScene(&m_ifcScene);
	
	m_objDB.openExisting("Assets/OBJ-DB.da");
	m_objScene.initialize("palace.obj", m_objDB);
	m_objScene.setScale(0.5f);
	m_objScene.setTranslation(glm::vec3(0.0f, -1.5f, 20.0f));
	m_renderer.addScene(&m_objScene);
	
	m_renderer.initialize(m_camera);

	m_fpsMeasurer.setLogFunction(5.0f, [](const FPSMeasurer& a_measurer) {
		print("FPS: %i\n", a_measurer.getAvgFps());
	});

	// Input stuffs
	m_windowQuitListener.setFunc([this]() { GLEngine::shutdown(); });
	m_keyDownListener.setFunc([this](EKey a_key) {
		m_cameraController.keyDown(a_key);
		if (a_key == EKey::ESCAPE)
			GLEngine::shutdown();
		else if (a_key == EKey::KP_9)
			m_renderer.setHBAOEnabled(!m_renderer.isHBAOEnabled());
		else if (a_key == EKey::KP_8)
			m_objScene.setVisibility(!m_objScene.isVisible());
		else if (a_key == EKey::KP_7)
			m_ifcScene.setVisibility(!m_ifcScene.isVisible());
		else if (a_key == EKey::KP_PLUS)
			m_cameraController.setCameraSpeed(m_cameraController.getCameraSpeed() * 1.1f);
		else if (a_key == EKey::KP_MINUS)
			m_cameraController.setCameraSpeed(m_cameraController.getCameraSpeed() * 0.9f);
		else if (a_key == EKey::T)
		{
			print("Created light\n");
			glm::vec3 position = m_camera.getPosition();
			glm::vec3 color = glm::normalize(glm::linearRand(glm::vec3(0.7), glm::vec3(1)));
			float intensity = glm::linearRand(0.7f, 2.5f);
			float radius = glm::linearRand(2.0f, 4.0f);
			m_lightManager.createLight(position, radius, color, intensity);
		}
		else if (a_key == EKey::Y)
			m_lightManager.deleteLights();
	});
	m_keyUpListener.setFunc([this](EKey a_key) {
		m_cameraController.keyUp(a_key);
	});
	m_mouseMovedListener.setFunc([this](uint xPos, uint yPos, int deltaX, int deltaY) {
		m_cameraController.mouseMoved(xPos, yPos, deltaX, deltaY);
	});
	m_mouseDownListener.setFunc([this](EMouseButton a_button, uint a_xPos, uint a_yPos) {
		m_cameraController.mouseDown(a_button, a_xPos, a_yPos);
	});
	m_mouseUpListener.setFunc([this](EMouseButton a_button, uint a_xPos, uint a_yPos) {
		m_cameraController.mouseUp(a_button, a_xPos, a_yPos);
	});
	//
}

DBTestScreen::~DBTestScreen()
{
}

void DBTestScreen::render(float a_deltaSec)
{
	if (!m_renderer.isHBAOEnabled())
		GLEngine::graphics->clear(glm::vec4(0.8f, 0.8f, 0.8f, 1.0f));

	//print("%f %f %f\n", m_camera.getPosition().x, m_camera.getPosition().y, m_camera.getPosition().z);
	m_cameraController.update(a_deltaSec);
	m_renderer.render(m_camera, m_lightManager);

	m_fpsMeasurer.tickFrame(a_deltaSec);

	GLEngine::graphics->swap();
}