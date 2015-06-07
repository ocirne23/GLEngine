#include "Screens/HBAOTestScreen.h"

#include "GLEngine.h"
#include "Graphics/Graphics.h"
#include "Graphics/PerspectiveCamera.h"
#include "Systems/CameraSystem.h"
#include "Systems/FPSControlSystem.h"
#include "Components/CameraComponent.h"
#include "Components/TransformComponent.h"
#include "Components/FPSControlledComponent.h"

HBAOTestScreen::HBAOTestScreen()
{
	m_entityx.systems.add<FPSControlSystem>();
	m_entityx.systems.add<CameraSystem>();
	m_entityx.systems.configure();

	m_camera = new PerspectiveCamera();
	m_camera->initialize((float) GLEngine::graphics->getViewportWidth(), (float) GLEngine::graphics->getViewportHeight(), 90.0f, 0.5f, 1500.0f);
	m_test.init(m_camera);

	entityx::Entity cameraEntity = m_entityx.entities.create();
	cameraEntity.assign<CameraComponent>(m_camera);
	cameraEntity.assign<TransformComponent>(0.0f, 0.0f, 0.0f);
	cameraEntity.assign<FPSControlledComponent>(10.0f, 0.7f);
}

HBAOTestScreen::~HBAOTestScreen()
{

}

void HBAOTestScreen::render(float a_deltaSec)
{
	m_entityx.systems.update<FPSControlSystem>(a_deltaSec);
	m_entityx.systems.update<CameraSystem>(a_deltaSec);

	m_test.render(a_deltaSec);
}