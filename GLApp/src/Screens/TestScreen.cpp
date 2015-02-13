#include "Screens/TestScreen.h"

#include "Components/CameraComponent.h"
#include "Components/ModelComponent.h"
#include "Components/PositionComponent.h"

#include "GLEngine.h"
#include "Graphics/Graphics.h"
#include "Graphics/PerspectiveCamera.h"
#include "Graphics/GL/GLMesh.h"
#include "Graphics/GL/GLVars.h"

#include "Model/ScreenManager.h"

#include "Systems/CameraSystem.h"
#include "Systems/LightSystem.h"
#include "Systems/RenderSystem.h"

BEGIN_UNNAMED_NAMESPACE()

static const char* const MODEL_FILE_PATH = "Models/palace/palace.da";

END_UNNAMED_NAMESPACE()

TestScreen::TestScreen()
{
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
	modelEntity.assign<ModelComponent>(m_mesh);
	modelEntity.assign<PositionComponent>(-10.0f, -10.0f, 50.0f);
}

TestScreen::~TestScreen() 
{
	SAFE_DELETE(m_mesh);
	SAFE_DELETE(m_camera);
}

void TestScreen::render(float a_deltaSec)
{
	m_entityx.systems.update<CameraSystem>(a_deltaSec);
	m_entityx.systems.update<LightSystem>(a_deltaSec);
	m_entityx.systems.update<RenderSystem>(a_deltaSec);
}

void TestScreen::show(uint a_width, uint a_height) {}
void TestScreen::resize(uint a_width, uint a_height) {}
void TestScreen::hide() {}