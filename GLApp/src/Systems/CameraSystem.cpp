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

	entityx::ComponentHandle<TransformComponent> transform = m_cameraEntity.component<TransformComponent>();
	entityx::ComponentHandle<CameraComponent> camera = m_cameraEntity.component<CameraComponent>();
	if (transform && camera)
	{
		glm::vec3 direction(0, 0, -1);
		glm::vec3 position(transform->transform[3]);
		glm::mat3 rotation(transform->transform);
		direction = direction * rotation;

		camera->camera->setPosition(position);
		camera->camera->lookAtDir(direction);
		camera->camera->update();
	}
}
