#include "Systems/CameraSystem.h"

#include "Components/CameraComponent.h"
#include "Components/TransformComponent.h"
#include "Graphics/PerspectiveCamera.h"

void CameraSystem::configure(entityx::EventManager& a_eventManager)
{
	a_eventManager.subscribe<entityx::ComponentAddedEvent<CameraComponent>>(*this);
}

void CameraSystem::receive(entityx::ComponentAddedEvent<CameraComponent>& a_cameraComponentAddedEvent)
{
	m_cameraEntity = a_cameraComponentAddedEvent.entity;
	m_activeCamera = &a_cameraComponentAddedEvent.component->camera;
}

void CameraSystem::update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt)
{
	if (!m_cameraEntity.valid())
		return;

	const glm::mat4& transform = m_cameraEntity.component<TransformComponent>()->transform;
	PerspectiveCamera& camera = m_cameraEntity.component<CameraComponent>()->camera;
	
	glm::vec3 direction(0, 0, -1);
	glm::vec3 position(transform[3]);
	glm::mat3 rotation(transform);
	direction = direction * rotation;

	camera.setPosition(position);
	camera.lookAtDir(direction);
	camera.updateMatrices();
}

const PerspectiveCamera* CameraSystem::getActiveCamera() const
{
	return m_cameraEntity.valid() ? m_activeCamera : NULL;
}
