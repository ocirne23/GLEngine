#include "Systems/CameraSystem.h"

#include "Components/CameraComponent.h"
#include "Components/TransformComponent.h"
#include "Graphics/PerspectiveCamera.h"

void CameraSystem::configure(entityx::EventManager& a_eventManager)
{
	a_eventManager.subscribe<entityx::ComponentAddedEvent<CameraComponent>>(*this);
}

void CameraSystem::receive(const entityx::ComponentAddedEvent<CameraComponent>& a_cameraComponentAddedEvent)
{
	m_cameraEntity = a_cameraComponentAddedEvent.entity;
}

void CameraSystem::update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt)
{
	if (!m_cameraEntity.valid())
		return;

	auto transformComponent = m_cameraEntity.component<TransformComponent>();
	auto cameraComponent = m_cameraEntity.component<CameraComponent>();
	if (transformComponent && cameraComponent)
	{
		glm::vec3 direction(0, 0, -1);
		glm::vec3 position(transformComponent->transform[3]);
		glm::mat3 rotation(transformComponent->transform);
		direction = direction * rotation;

		cameraComponent->camera->setPosition(position);
		cameraComponent->camera->lookAtDir(direction);
		cameraComponent->camera->updateMatrices();
	}
}
