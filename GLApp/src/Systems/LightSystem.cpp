#include "Systems/LightSystem.h"

#include "Components/CameraComponent.h"
#include "Components/TransformComponent.h"
#include "Components/PointLightComponent.h"
#include "GLEngine.h"
#include "Systems/CameraSystem.h"

LightSystem::LightSystem(const CameraSystem& cameraSystem) : m_cameraSystem(cameraSystem), m_lightManager(MAX_LIGHTS)
{
	m_viewspaceLightPositionRangeList = new glm::vec4[m_lightManager.getMaxLights()];
}

LightSystem::~LightSystem()
{
	SAFE_DELETE_ARRAY(m_viewspaceLightPositionRangeList);
}

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
		updateViewspaceLightPositionRangeList(*activeCamera);
}

uint LightSystem::getNumLights() const
{
	return m_lightManager.getNumLights();
}

const glm::vec4* LightSystem::getViewspaceLightPositionRangeList() const
{
	return m_lightManager.getNumLights() ? &m_viewspaceLightPositionRangeList[0] : NULL;
}

const glm::vec4* LightSystem::getLightColorIntensityList() const
{
	return m_lightManager.getLightColorIntensities();
}

void LightSystem::updateViewspaceLightPositionRangeList(const PerspectiveCamera& a_camera)
{
	const glm::vec4* lightPositionRanges = m_lightManager.getLightPositionRanges();
	uint numUsedLights = m_lightManager.getNumLights();
	for (uint i = 0; i < m_lightManager.getNumLights(); ++i)
	{
		m_viewspaceLightPositionRangeList[i] = glm::vec4(a_camera.getViewMatrix() * glm::vec4(glm::vec3(lightPositionRanges[i]), 1.0));
		m_viewspaceLightPositionRangeList[i].w = lightPositionRanges[i].w;
	}
}

