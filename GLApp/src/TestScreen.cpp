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
	m_camera.lookAtPoint(glm::vec3(0.0f, 0.0f, 20.0f));
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
	m_objDB.openExisting("assets/OBJ-DB.da");

	m_palaceScene.initialize("palace.obj", m_objDB);
	m_palaceScene.setScale(0.5f);
	m_palaceScene.setTranslation(glm::vec3(0.0f, -1.5f, 20.0f));
	m_renderer.addScene(&m_palaceScene);

	DBScene sphereDB("assets-source/Models/sphere/sphere.obj");
	createTestSphere(sphereDB, "Gold", glm::vec3(10, 2.0f, 10));
	createTestSphere(sphereDB, "Plywood", glm::vec3(10, 2.0f, 14));
	createTestSphere(sphereDB, "Iron", glm::vec3(10, 2.0f, 18));
	createTestSphere(sphereDB, "Plastic", glm::vec3(10, 2.0f, 22));

	m_skysphereScene.initialize("skysphere.obj", m_objDB);
	m_skysphereScene.setRotation(glm::vec3(0, 1, 0), 0);
	m_renderer.addSkybox(&m_skysphereScene);

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
		case EKey::KP_5:     m_renderer.setShadowsEnabled(!m_renderer.isShadowsEnabled()); break;
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
	m_renderer.setSun(glm::mat3(sunRot) * baseSunDir, glm::vec3(0.75f, 0.7f, 0.66f), 1.0f);
	m_skysphereScene.setRotation(glm::vec3(0, 1, 0), a_angle - 140);
}

void TestScreen::createTestSphere(DBScene& a_sphereDB, const eastl::string& a_materialName, const glm::vec3& a_position)
{
	a_sphereDB.swapMaterial("Placeholder", a_materialName);
	GLScene* sphere = new GLScene();
	sphere->initialize(a_sphereDB);
	sphere->setScale(1.5f);
	sphere->setTranslation(a_position);
	m_renderer.addScene(sphere);
	a_sphereDB.swapMaterial(a_materialName, "Placeholder");
}
