#include "Screens/TestScreen.h"

#include "Components/CameraComponent.h"
#include "Components/FPSControlledComponent.h"
#include "Components/ModelComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SkyComponent.h"
#include "Components/TransformComponent.h"

#include "Editor/Editor.h"

#include "GLEngine.h"
#include "Graphics/Graphics.h"
#include "Graphics/PerspectiveCamera.h"
#include "Graphics/GL/GLMesh.h"
#include "Graphics/GL/GLVars.h"

#include "Input/Input.h"

#include "Systems/CameraSystem.h"
#include "Systems/FPSControlSystem.h"
#include "Systems/LightSystem.h"
#include "Systems/RenderSystem.h"

#include "Utils/ListenerMacros.h"

BEGIN_UNNAMED_NAMESPACE()

static const char* const MODEL_FILE_PATH = "Models/palace/palace.da";

END_UNNAMED_NAMESPACE()

TestScreen::TestScreen()
{
	GLEngine::input->keyDownListenerRegister(this, [&](EKey a_key) { return keyDown(a_key); });
	GLEngine::input->windowQuitListenerRegister(this, []() { print("shutting down\n"); GLEngine::shutdown(); });

	m_entityx.systems.add<FPSControlSystem>();
	m_entityx.systems.add<CameraSystem>();
	m_entityx.systems.add<LightSystem>();
	m_entityx.systems.add<RenderSystem>(*m_entityx.systems.system<LightSystem>());
	m_entityx.systems.configure();

	m_camera = new PerspectiveCamera();
	m_camera->initialize((float) GLEngine::graphics->getScreenWidth(), (float) GLEngine::graphics->getScreenHeight(), 90.0f, 0.5f, 1500.0f);

	m_building = new GLMesh();
	m_building->loadFromFile(MODEL_FILE_PATH, TextureUnits::MODEL_TEXTURE_ARRAY, UBOBindingPoints::MODEL_MATERIAL_UBO_BINDING_POINT);

	m_skybox = new GLMesh();
	m_skybox->loadFromFile("Models/Skybox/skysphere.da", TextureUnits::MODEL_TEXTURE_ARRAY, UBOBindingPoints::MODEL_MATERIAL_UBO_BINDING_POINT);

	entityx::Entity cameraEntity = m_entityx.entities.create();
	cameraEntity.assign<CameraComponent>(m_camera);
	cameraEntity.assign<TransformComponent>(0.0f, 0.0f, 0.0f);
	cameraEntity.assign<FPSControlledComponent>(10.0f, 0.7f);

	entityx::Entity modelEntity = m_entityx.entities.create();
	modelEntity.assign<ModelComponent>(m_building);
	modelEntity.assign<TransformComponent>(0.0f, -10.0f, -70.0f);

	entityx::Entity lightEntity = m_entityx.entities.create();
	lightEntity.assign<PointLightComponent>()->set(glm::vec3(0, -10.0f, -20.0f), 5.0f, glm::vec3(1.0f), 20.0f);

	entityx::Entity skyboxEntity = m_entityx.entities.create();
	skyboxEntity.assign<SkyComponent>(m_skybox);
	skyboxEntity.assign<TransformComponent>();
}

TestScreen::~TestScreen()
{
	SAFE_DELETE(m_building);
	SAFE_DELETE(m_camera);

	GLEngine::input->keyDownListenerUnregister(this);
	GLEngine::input->windowQuitListenerUnregister(this);
}

void TestScreen::render(float a_deltaSec)
{
	int lightAnimationOffset = 0;
	entityx::ComponentHandle<PointLightComponent> light;
	for (entityx::Entity e : m_entityx.entities.entities_with_components(light))
	{
		light->setRadius(2.0f + 20.0f * (((GLEngine::getTimeMs() + lightAnimationOffset * 12345) % 1000) / 1000.0f));
		light->setIntensity(2.0f + 80.0f * (((GLEngine::getTimeMs() + lightAnimationOffset * 54321) % 10000) / 10000.0f));
		++lightAnimationOffset;
	}
	m_entityx.systems.update<FPSControlSystem>(a_deltaSec);
	m_entityx.systems.update<CameraSystem>(a_deltaSec);
	m_entityx.systems.update<LightSystem>(a_deltaSec);
	m_entityx.systems.update<RenderSystem>(a_deltaSec);
}

bool TestScreen::keyDown(EKey a_key)
{
	if (a_key == EKey::T)
	{
		glm::vec3 position = m_camera->getPosition() + m_camera->getDirection();
		glm::vec3 color = glm::normalize(glm::vec3((rand() % 1000) / 1000.0f, (rand() % 1000) / 1000.0f, (rand() % 1000) / 1000.0f));
		float intensity = 20.0f * ((rand() % 1000) / 1000.0f) + 1.0f;
		float radius = 20.0f * ((rand() % 1000) / 1000.0f) + 1.0f;

		entityx::Entity lightEntity = m_entityx.entities.create();
		lightEntity.assign<PointLightComponent>()->set(position, radius, color, intensity);

#ifdef EDITOR
		GLEngine::editor->sendTest0Message();
#endif // EDITOR

		return true;
	}
	if (a_key == EKey::Y)
	{
		for (entityx::Entity e : m_entityx.entities.entities_with_components<PointLightComponent>())
		{
			e.component<PointLightComponent>().remove();
			e.destroy();
		}

#ifdef EDITOR
		GLEngine::editor->sendTest1Message();
#endif // EDITOR

		return true;
	}
	return false;
}