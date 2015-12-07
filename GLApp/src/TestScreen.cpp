#include "TestScreen.h"

#include "GLEngine.h"
#include "Graphics/Graphics.h"
#include "Graphics/GL/Scene/GLConfig.h"

#include <glm/gtc/random.hpp>

TestScreen::TestScreen() : m_lightManager(GLConfig::MAX_LIGHTS)
{
	uint viewportWidth = GLEngine::graphics->getViewportWidth();
	uint viewportHeight = GLEngine::graphics->getViewportHeight();
	m_camera.initialize((float) viewportWidth, (float) viewportHeight, 90.0f, 0.1f, 1000.0f);
	m_camera.setPosition(4.0f, 1.0f, 4.0f);
	m_camera.lookAtPoint(-4.0f, 6.0f, -4.0f);
	m_renderer.initialize(m_camera);

	m_ifcDB.openExisting("Assets/IFC-DB.da");
	m_ifcScene.initialize("ifc2.ifc", m_ifcDB);
	m_renderer.addScene(&m_ifcScene);
	
	m_objDB.openExisting("Assets/OBJ-DB.da");
	m_objScene.initialize("palace.obj", m_objDB);
	m_objScene.setScale(0.5f);
	m_objScene.setTranslation(glm::vec3(0.0f, -1.5f, 20.0f));
	m_renderer.addScene(&m_objScene);

	m_fpsMeasurer.setLogFunction(10.0f, [this]() { print("FPS: %i\n", m_fpsMeasurer.getAvgFps()); });

	// Input stuffs
	m_windowQuitListener.setFunc([]() { GLEngine::shutdown(); });
	m_keyDownListener.setFunc([this](EKey a_key)
	{
		switch (a_key)
		{
		case EKey::ESCAPE:   GLEngine::shutdown(); break;
		case EKey::KP_9:     m_renderer.setHBAOEnabled(!m_renderer.isHBAOEnabled()); break;
		case EKey::KP_8:     m_objScene.setVisibility(!m_objScene.isVisible()); break;
		case EKey::KP_7:     m_ifcScene.setVisibility(!m_ifcScene.isVisible()); break;
		case EKey::KP_PLUS:  m_cameraController.setCameraSpeed(m_cameraController.getCameraSpeed() * 1.2f); break;
		case EKey::KP_MINUS: m_cameraController.setCameraSpeed(m_cameraController.getCameraSpeed() * 0.8f); break;
		case EKey::Y:        m_lightManager.deleteLights(); break;
		case EKey::T:
		{
			glm::vec3 position = m_camera.getPosition();
			glm::vec3 color = glm::normalize(glm::linearRand(glm::vec3(0.7), glm::vec3(1)));
			float intensity = glm::linearRand(0.7f, 2.5f);
			float radius = glm::linearRand(2.0f, 4.0f);
			m_lightManager.createLight(position, radius, color, intensity);
			break;
		}
		}
	});
}

void TestScreen::render(float a_deltaSec)
{
	if (!m_renderer.isHBAOEnabled())
		GLEngine::graphics->clear(glm::vec4(0.8f, 0.8f, 0.8f, 1.0f));

	m_cameraController.update(m_camera, a_deltaSec);
	m_renderer.render(m_camera, m_lightManager);

	m_fpsMeasurer.tickFrame(a_deltaSec);

	GLEngine::graphics->swap();
}