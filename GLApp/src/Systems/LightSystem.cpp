#include "Systems/LightSystem.h"

#include "Components/CameraComponent.h"

// TEMP INCLUDES
#include "GLEngine.h"
#include "Graphics/PerspectiveCamera.h"
#include "Input/Input.h"
#include "rde/vector.h"
// !TEMP INCLUDES

LightSystem::LightSystem() : m_lightManager(MAX_LIGHTS)
{
	GLEngine::input->registerKeyListener(this);
}

LightSystem::~LightSystem()
{
	GLEngine::input->unregisterKeyListener(this);
}

void LightSystem::configure(entityx::EventManager& a_eventManager)
{
	a_eventManager.subscribe<entityx::ComponentAddedEvent<CameraComponent>>(*this);
	a_eventManager.subscribe<entityx::ComponentRemovedEvent<CameraComponent>>(*this);
}

void LightSystem::receive(const entityx::ComponentAddedEvent<CameraComponent>& a_cameraComponentAddedEvent)
{
	assert(!m_activeCamera);
	m_activeCamera = a_cameraComponentAddedEvent.component->camera;
}

void LightSystem::receive(const entityx::ComponentRemovedEvent<CameraComponent>& a_cameraComponentRemovedEvent)
{
	assert(m_activeCamera == a_cameraComponentRemovedEvent.component->camera);
	m_activeCamera = NULL;
}

void LightSystem::update(entityx::EntityManager& a_entities, entityx::EventManager& a_events, entityx::TimeDelta a_dt)
{
	// TMP Light range oscilation
	glm::vec4* lights = m_lightManager.getLightPositionRanges();
	for (uint i = 0; i < m_lightManager.getNumLights(); ++i)
	{
		lights[i].w = 2.0f + 10.0f * (((GLEngine::getTimeMs() + i * 12345) % 2000) / 2000.0f);
	}
	//

	m_viewspaceLightPositionRangeList = m_lightManager.updateViewspaceLightPositionRangeList(*m_activeCamera);
}

uint LightSystem::getNumLights()
{
	return m_lightManager.getNumLights();
}

const glm::vec4* LightSystem::getViewspaceLightPositionRangeList()
{
	return m_viewspaceLightPositionRangeList;
}

const glm::vec4* LightSystem::getLightColorList()
{
	return m_lightManager.getLightColors();
}

// TEMP LIGHT PLACEMENT
BEGIN_UNNAMED_NAMESPACE()
rde::vector<LightHandle> lights;
END_UNNAMED_NAMESPACE()

bool LightSystem::keyDown(Key a_key)
{
	if (!m_activeCamera)
		return false;

	switch (a_key)
	{
	case Key_T:
	{
		glm::vec3 lightColor = glm::normalize(glm::vec3((rand() % 1000) / 1000.0f, (rand() % 1000) / 1000.0f, (rand() % 1000) / 1000.0f));
		lightColor *= rand() % 1000 / 200.0f;
		LightHandle h = m_lightManager.createLight(m_activeCamera->m_position, lightColor, 5.0f);
		lights.push_back(h);
		return true;
	}
	case Key_Y:
	{
		if (lights.size())
		{
			LightHandle h = lights.back();
			m_lightManager.deleteLight(h);
			lights.pop_back();
		}
		return true;
	}
	}
	return false;
}
// !TEMP LIGHT PLACEMENT