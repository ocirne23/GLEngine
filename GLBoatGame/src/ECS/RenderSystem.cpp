#include "ECS/RenderSystem.h"

#include "GLEngine.h"
#include "Graphics/Graphics.h"

#include "BoatGame.h"
#include "ECS/PhysicsSystem.h"
#include "Utils/StringUtils.h"

#include <glm/glm.hpp>

RenderSystem::RenderSystem(BoatGame& a_boatGame)
	: m_boatGame(a_boatGame)
{
	m_boatGame.getPhysicsSystem().initializeDebugDraw();

	m_camera.initialize(scast<float>(GLEngine::graphics->getViewportWidth()), scast<float>(GLEngine::graphics->getViewportHeight()), 90.0f, 0.1f, 1000.0f);
	//m_cameraController.setCameraSpeed(5.0f);
	m_camera.setPosition({ 0, 0, 10 });
	m_camera.lookAtPoint({ 0, 0, 0 });
	m_camera.updateMatrices();

	m_fpsMeasurer.setLogFunction(1.0f, [this]() {
		GLEngine::graphics->setWindowTitle(("GPBoatGame FPS: " + StringUtils::to_string(m_fpsMeasurer.getAvgFps())).c_str());
	});
}

RenderSystem::~RenderSystem()
{
}

void RenderSystem::render(float a_deltaSec)
{
	m_fpsMeasurer.tickFrame(a_deltaSec);
	GLEngine::graphics->clear(glm::vec4(0.2f, 0.2f, 0.4f, 1.0f));
	//m_cameraController.update(m_camera, a_deltaSec);
	m_boatGame.getPhysicsSystem().drawDebugInfo(m_camera);
	GLEngine::graphics->swap();
}
