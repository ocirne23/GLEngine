#include "TestScreen.h"

#include "GLEngine.h"
#include "Graphics/Graphics.h"
#include "Graphics/GL/Scene/GLConfig.h"
#include "Utils/StringUtils.h"
#include "Database/Assets/DBScene.h"

#include <glm/gtc/random.hpp>
#include <glm/gtx/rotate_vector.hpp>

TestScreen::TestScreen() : m_lightManager(GLConfig::MAX_LIGHTS)
{
	uint viewportWidth = GLEngine::graphics->getViewportWidth();
	uint viewportHeight = GLEngine::graphics->getViewportHeight();
	m_camera.initialize((float) viewportWidth, (float) viewportHeight, 90.0f, 0.1f, 1000.0f);
	m_camera.setPosition(4.0f, 1.0f, 4.0f);
	m_camera.lookAtPoint(-4.0f, 6.0f, -4.0f);
	m_renderer.initialize(m_camera);
	m_cameraController.setCameraSpeed(5.0f);

	m_ifcDB.openExisting("Assets/IFC-DB.da");
	if (m_ifcDB.isOpen())
	{
		if (m_ifcDB.hasAsset("ifc1.ifc"))
		{
			m_ifc1Scene.initialize("ifc1.ifc", m_ifcDB);
			m_ifc1Scene.setTranslation(glm::vec3(10, -0.1, -35));
			m_renderer.addScene(&m_ifc1Scene);
		}
		if (m_ifcDB.hasAsset("ifc2.ifc"))
		{
			m_ifc2Scene.initialize("ifc2.ifc", m_ifcDB);
			m_renderer.addScene(&m_ifc2Scene);
		}
	}

	m_objDB.openExisting("Assets/OBJ-DB.da");
	if (m_objDB.isOpen())
	{
		if (m_objDB.hasAsset("palace.obj"))
		{
			m_palaceScene.initialize("palace.obj", m_objDB);
			m_palaceScene.setScale(0.5f);
			m_palaceScene.setTranslation(glm::vec3(0.0f, -1.5f, 20.0f));
			m_renderer.addScene(&m_palaceScene);
		}
		if (m_objDB.hasAsset("skysphere.obj"))
		{
			m_skysphereScene.initialize("skysphere.obj", m_objDB);
			m_skysphereScene.setRotation(glm::vec3(0, 1, 0), 0);
			m_renderer.addSkybox(&m_skysphereScene);
		}
		if (m_objDB.hasAsset("sphere.obj"))
		{
			
			m_sphereScene.initialize("sphere.obj", m_objDB);
			m_sphereScene.setScale(0.5f);
			m_sphereScene.setTranslation(glm::vec3(-2, 0.5f, -5));
			m_renderer.addScene(&m_sphereScene);
		}
	}
	m_fpsMeasurer.setLogFunction(1.0f, [this]() { GLEngine::graphics->setWindowTitle(("GLApp FPS: " + StringUtils::to_string(m_fpsMeasurer.getAvgFps())).c_str()); });
	setSunRotation(m_sunRotation);

	// Input stuffs
	m_windowQuitListener.setFunc([]() { GLEngine::shutdown(); });
	m_keyDownListener.setFunc([this](EKey a_key)
	{
		switch (a_key)
		{
		case EKey::ESCAPE:   GLEngine::shutdown(); break;
		case EKey::KP_1:     m_renderer.setHBAOEnabled(!m_renderer.isHBAOEnabled()); break;
		case EKey::KP_2:     m_renderer.setFXAAEnabled(!m_renderer.isFXAAEnabled()); break;
		case EKey::KP_3:     m_renderer.setBloomEnabled(!m_renderer.isBloomEnabled()); break;
		case EKey::KP_4:     m_renderer.setDepthPrepassEnabled(!m_renderer.isDepthPrepassEnabled()); break;
		case EKey::KP_5:     m_sunRotation = 0.0f; break;
		case EKey::KP_6:     m_renderer.reloadShaders(); break;
		
		case EKey::KP_9:     m_palaceScene.setVisibility(!m_palaceScene.isVisible()); break;
		case EKey::KP_8:     m_ifc1Scene.setVisibility(!m_ifc1Scene.isVisible()); break;
		case EKey::KP_7:     m_ifc2Scene.setVisibility(!m_ifc2Scene.isVisible()); break;

		case EKey::KP_PLUS:  m_cameraController.setCameraSpeed(m_cameraController.getCameraSpeed() * 1.2f); break;
		case EKey::KP_MINUS: m_cameraController.setCameraSpeed(m_cameraController.getCameraSpeed() * 0.8f); break;
		case EKey::Y:        m_lightManager.deleteLights(); break;
		case EKey::T:
		{
			glm::vec3 position = m_camera.getPosition();
			glm::vec3 color = glm::normalize(glm::linearRand(glm::vec3(0.5), glm::vec3(1)));
			float intensity = glm::linearRand(0.8f, 2.0f);
			float radius = glm::linearRand(40.0f, 80.0f);
			m_lightManager.createLight(position, radius, color, intensity);
			break;
		}
		}
	});
}

void TestScreen::render(float a_deltaSec)
{
	m_timeAccum += a_deltaSec;
	if (m_timeAccum > 1.0f)
	{
		m_timeAccum = 0.0f;
		setSunRotation(m_sunRotation);
		m_sunRotation += 1.0f;
	}

	m_fpsMeasurer.tickFrame(a_deltaSec);
	m_cameraController.update(m_camera, a_deltaSec);
	m_renderer.render(m_camera, m_lightManager);
	GLEngine::graphics->swap();
}

void TestScreen::setSunRotation(float a_angle)
{
	glm::vec3 baseSunDir = glm::normalize(glm::vec3(0.597, 0.537, 0.597));
	glm::mat4 sunRot = glm::rotate(a_angle, glm::vec3(0, 1, 0));
	m_renderer.setSun(glm::mat3(sunRot) * baseSunDir, glm::vec3(0.75f, 0.70f, 0.66f), 1.0f);
	m_skysphereScene.setRotation(glm::vec3(0, 1, 0), a_angle - 140);
}
