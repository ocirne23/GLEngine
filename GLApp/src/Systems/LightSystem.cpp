#include "Systems/LightSystem.h"

#include "Components/CameraComponent.h"
#include "Components/TransformComponent.h"
#include "Components/PointLightComponent.h"

#include "GLEngine.h"

void LightSystem::configure(entityx::EventManager& a_eventManager)
{
    a_eventManager.subscribe<entityx::ComponentAddedEvent<CameraComponent>>(*this);
    a_eventManager.subscribe<entityx::ComponentAddedEvent<PointLightComponent>>(*this);
    a_eventManager.subscribe<entityx::ComponentRemovedEvent<PointLightComponent>>(*this);
}

void LightSystem::receive(const entityx::ComponentAddedEvent<CameraComponent>& a_cameraComponentAddedEvent)
{
    assert(!m_activeCamera);
    m_activeCamera = a_cameraComponentAddedEvent.component->camera;
}

void LightSystem::receive(const entityx::ComponentAddedEvent<PointLightComponent>& pointLightComponentAddedEvent)
{
    auto light = pointLightComponentAddedEvent.component;
    light->m_lightManager = &m_lightManager;
    light->m_lightHandle = m_lightManager.createLight();
}

void LightSystem::receive(const entityx::ComponentRemovedEvent<PointLightComponent>& pointLightComponentRemovedEvent)
{
    m_lightManager.deleteLight(pointLightComponentRemovedEvent.component->m_lightHandle);
}

void LightSystem::update(entityx::EntityManager& a_entities, entityx::EventManager& a_events, entityx::TimeDelta a_dt)
{
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

const glm::vec4* LightSystem::getLightColorIntensityList()
{
    return m_lightManager.getLightColorIntensities();
}