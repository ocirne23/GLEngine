#include "Systems/LightSystem.h"

#include "Components/CameraComponent.h"
#include "Components/TransformComponent.h"
#include "Components/PointLightComponent.h"

#include "GLEngine.h"

LightSystem::LightSystem() : m_lightManager(MAX_LIGHTS)
{
}

LightSystem::~LightSystem()
{
}

void LightSystem::configure(entityx::EventManager& a_eventManager)
{
	a_eventManager.subscribe<entityx::ComponentAddedEvent<CameraComponent>>(*this);
	a_eventManager.subscribe<entityx::ComponentRemovedEvent<CameraComponent>>(*this);

	a_eventManager.subscribe<entityx::ComponentAddedEvent<PointLightComponent>>(*this);
	a_eventManager.subscribe<entityx::ComponentRemovedEvent<PointLightComponent>>(*this);
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

void LightSystem::receive(const entityx::ComponentAddedEvent<PointLightComponent>& pointLightComponentAddedEvent)
{
	m_addedPointLightEntities.push_back(pointLightComponentAddedEvent.entity);
}

void LightSystem::receive(const entityx::ComponentRemovedEvent<PointLightComponent>& pointLightComponentRemovedEvent)
{
	m_lightManager.deleteLight(pointLightComponentRemovedEvent.component->lightHandle);
}

void LightSystem::update(entityx::EntityManager& a_entities, entityx::EventManager& a_events, entityx::TimeDelta a_dt)
{
	if (m_addedPointLightEntities.size())
	{
		for (entityx::Entity& e : m_addedPointLightEntities)
		{
			assert(e.valid());
			entityx::ComponentHandle<TransformComponent> transform = e.component<TransformComponent>();
			entityx::ComponentHandle<PointLightComponent> light = e.component<PointLightComponent>();
			assert(transform.valid());
			assert(light.valid());

			glm::vec3 pos(transform->transform[3]);
			glm::vec3 lightColor = glm::normalize(glm::vec3((rand() % 1000) / 1000.0f, (rand() % 1000) / 1000.0f, (rand() % 1000) / 1000.0f)) * (rand() % 1000 / 200.0f);
			light->lightHandle = m_lightManager.createLight(pos, lightColor, 5.0f);
		}
		m_addedPointLightEntities.clear();
	}

	// TMP Light range oscilation
	glm::vec4* lights = m_lightManager.getLightPositionRanges();
	for (uint i = 0; i < m_lightManager.getNumLights(); ++i)
		lights[i].w = 2.0f + 10.0f * (((GLEngine::getTimeMs() + i * 12345) % 2000) / 2000.0f);
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