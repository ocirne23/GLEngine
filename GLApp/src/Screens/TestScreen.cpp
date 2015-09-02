#include "Screens/TestScreen.h"

#include "3rdparty/rde/rde_string.h"

#include "Components/CameraComponent.h"
#include "Components/FPSControlledComponent.h"
#include "Components/ModelComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SkyComponent.h"
#include "Components/TransformComponent.h"

#include "GLEngine.h"
#include "Graphics/Graphics.h"

#include "Input/Input.h"

#include "Systems/CameraSystem.h"
#include "Systems/FPSControlSystem.h"
#include "Systems/LightSystem.h"
#include "Systems/ModelManageSystem.h"
#include "Systems/RenderSystem.h"

#include <glm/gtc/random.hpp>

BEGIN_UNNAMED_NAMESPACE()

static const rde::string MODEL_FILE_PATH("Models/palace/palace.obj.da");
static const rde::string MODEL2_FILE_PATH("Models/meshes/dragon.obj.da");
static const rde::string SKYBOX_FILE_PATH("Models/skybox/skysphere.obj.da");

END_UNNAMED_NAMESPACE()

using namespace entityx;

TestScreen::TestScreen()
{
	uint viewportWidth = GLEngine::graphics->getViewportWidth();
	uint viewportHeight = GLEngine::graphics->getViewportHeight();

	m_keyDownListener.setFunc([this](EKey a_key) { keyDown(a_key); });
	m_windowQuitListener.setFunc([this]() { GLEngine::shutdown(); });

	SystemManager& systems = m_entityx.systems;
	systems.add<ModelManageSystem>();
	systems.add<FPSControlSystem>();
	systems.add<CameraSystem>();
	systems.add<LightSystem>(*systems.system<CameraSystem>());
	systems.add<RenderSystem>(*systems.system<CameraSystem>(), *systems.system<LightSystem>());
	systems.configure();

	Entity cameraEntity = m_entityx.entities.create();
	cameraEntity.assign<CameraComponent>((float) viewportWidth, (float) viewportHeight, 90.0f, 0.5f, 1000.0f);
	cameraEntity.assign<TransformComponent>(0.0f, 0.0f, 0.0f);
	cameraEntity.assign<FPSControlledComponent>(10.0f, 0.7f);

	Entity skyboxEntity = m_entityx.entities.create();
	skyboxEntity.assign<SkyComponent>();
	skyboxEntity.assign<ModelComponent>(SKYBOX_FILE_PATH);

	Entity modelEntity = m_entityx.entities.create();
	modelEntity.assign<ModelComponent>(MODEL_FILE_PATH);
	modelEntity.assign<TransformComponent>(0.0f, -10.0f, -70.0f);

	Entity model2Entity = m_entityx.entities.create();
	model2Entity.assign<ModelComponent>(MODEL2_FILE_PATH);
	model2Entity.assign<TransformComponent>(0.0f, -7.0f, -58.0f);

	Entity lightEntity = m_entityx.entities.create();
	lightEntity.assign<PointLightComponent>()->set(glm::vec3(0, -10.0f, -20.0f), 5.0f, glm::vec3(1.0f), 20.0f);
}

TestScreen::~TestScreen()
{
}

void TestScreen::render(float a_deltaSec)
{
	SystemManager& systems = m_entityx.systems;
	systems.update<FPSControlSystem>(a_deltaSec);
	systems.update<CameraSystem>(a_deltaSec);
	systems.update<LightSystem>(a_deltaSec);
	systems.update<RenderSystem>(a_deltaSec);
}

void TestScreen::keyDown(EKey a_key)
{
	SystemManager& systems = m_entityx.systems;

	switch (a_key)
	{
	case EKey::T:
	{
		const PerspectiveCamera* camera = systems.system<CameraSystem>()->getActiveCamera();
		// Place light 1.0f in front of camera position.
		glm::vec3 position = camera->getPosition() + camera->getDirection();
		glm::vec3 color = glm::normalize(glm::linearRand(glm::vec3(0), glm::vec3(1)));
		float intensity = glm::linearRand(7.0f, 25.0f);
		float radius = glm::linearRand(10.0f, 20.0f);

		entityx::Entity lightEntity = m_entityx.entities.create();
		lightEntity.assign<PointLightComponent>()->set(position, radius, color, intensity);
		m_lightEntities.push_back(lightEntity);
		break;
	}
	case EKey::Y:
	{
		if (m_lightEntities.size())
		{
			m_lightEntities.back().destroy();
			m_lightEntities.pop_back();
		}
		break;
	}
	case EKey::G:
	{
		auto renderSystem = systems.system<RenderSystem>();
		renderSystem->setHBAOEnabled(!renderSystem->isHBAOEnabled());
		break;
	}
	case EKey::ESCAPE:
	{
		GLEngine::shutdown();
		break;
	}
	}
}