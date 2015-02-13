#include "Screens/TestScreen.h"

#include "Components/CameraComponent.h"
#include "Components/FPSControlledComponent.h"
#include "Components/ModelComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/TransformComponent.h"

#include "GLEngine.h"
#include "Graphics/Graphics.h"
#include "Graphics/PerspectiveCamera.h"
#include "Graphics/GL/GLMesh.h"
#include "Graphics/GL/GLVars.h"

#include "Input/Input.h"

#include "Model/ScreenManager.h"

#include "Systems/CameraSystem.h"
#include "Systems/FPSControlSystem.h"
#include "Systems/LightSystem.h"
#include "Systems/RenderSystem.h"

BEGIN_UNNAMED_NAMESPACE()

static const char* const MODEL_FILE_PATH = "Models/palace/palace.da";

END_UNNAMED_NAMESPACE()

TestScreen::TestScreen()
{
	m_entityx.systems.add<FPSControlSystem>();
	m_entityx.systems.add<CameraSystem>();
	m_entityx.systems.add<LightSystem>();
	m_entityx.systems.add<RenderSystem>(*m_entityx.systems.system<LightSystem>());
	m_entityx.systems.configure();

	m_camera = new PerspectiveCamera();
	m_camera->initialize(glm::vec3(0, 0, 0), glm::vec3(0, 0, 1), (float) GLEngine::graphics->getScreenWidth(), (float) GLEngine::graphics->getScreenHeight(), 90.0f, 0.5f, 1500.0f);

	m_mesh = new GLMesh();
	m_mesh->loadFromFile(MODEL_FILE_PATH, GLVars::TextureUnits_MODEL_TEXTURE_ARRAY, GLVars::UBOBindingPoints_MODEL_MATERIAL_UBO_BINDING_POINT);
	
	entityx::Entity cameraEntity = m_entityx.entities.create();
	entityx::Entity modelEntity = m_entityx.entities.create();

	cameraEntity.assign<CameraComponent>(m_camera);
	cameraEntity.assign<TransformComponent>(0.0f, 0.0f, 0.0f);
	cameraEntity.assign<FPSControlledComponent>(10.0f, 0.7f);

	modelEntity.assign<ModelComponent>(m_mesh);
	modelEntity.assign<TransformComponent>(0.0f, -10.0f, -70.0f);

	GLEngine::input->registerKeyListener(this);
}

TestScreen::~TestScreen() 
{
	SAFE_DELETE(m_mesh);
	SAFE_DELETE(m_camera);
	GLEngine::input->unregisterKeyListener(this);
}

bool TestScreen::keyDown(Key a_key)
{
	if (a_key == Key_T)
	{
		entityx::Entity lightEntity = m_entityx.entities.create();
		lightEntity.assign<TransformComponent>(m_camera->m_position + m_camera->m_direction);
		lightEntity.assign<PointLightComponent>();
		return true;
	}
	if (a_key == Key_Y)
	{
		for (entityx::Entity e : m_entityx.entities.entities_with_components<PointLightComponent>())
		{
			e.component<PointLightComponent>().remove();
			e.destroy();
		}
		return true;
	}
	return false;
}


void TestScreen::render(float a_deltaSec)
{
	m_entityx.systems.update<FPSControlSystem>(a_deltaSec);
	m_entityx.systems.update<CameraSystem>(a_deltaSec);
	m_entityx.systems.update<LightSystem>(a_deltaSec);
	m_entityx.systems.update<RenderSystem>(a_deltaSec);
}

void TestScreen::show(uint a_width, uint a_height) {}
void TestScreen::resize(uint a_width, uint a_height) {}
void TestScreen::hide() {}