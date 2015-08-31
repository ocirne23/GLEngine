#include "Systems/LightSystem.h"

#include "Components/CameraComponent.h"
#include "Components/TransformComponent.h"
#include "Components/PointLightComponent.h"
#include "GLEngine.h"
#include "Systems/CameraSystem.h"

void LightSystem::configure(entityx::EventManager& a_eventManager)
{
	a_eventManager.subscribe<entityx::ComponentAddedEvent<PointLightComponent>>(*this);
}

void LightSystem::receive(entityx::ComponentAddedEvent<PointLightComponent>& pointLightComponentAddedEvent)
{
	auto lightComponent = pointLightComponentAddedEvent.component;
	// Set the mutable field of lightComponent to allow for using set()
	lightComponent->m_lightManager = &m_lightManager;
	lightComponent->m_lightHandle = m_lightManager.createLight();
}

void LightSystem::update(entityx::EntityManager& a_entities, entityx::EventManager& a_events, entityx::TimeDelta a_dt)
{
	const auto activeCamera = m_cameraSystem.getActiveCamera();
	if (activeCamera)
		m_viewspaceLightPositionRangeList = m_lightManager.updateViewspaceLightPositionRangeList(*activeCamera);
}

uint LightSystem::getNumLights() const
{
	return m_lightManager.getNumLights();
}

const glm::vec4* LightSystem::getViewspaceLightPositionRangeList() const
{
	return m_viewspaceLightPositionRangeList;
}

const glm::vec4* LightSystem::getLightColorIntensityList() const
{
	return m_lightManager.getLightColorIntensities();
}