#pragma once

#include "Core.h"

#include "3rdparty/entityx/System.h"

class PerspectiveCamera;
struct CameraComponent;

class CameraSystem : public entityx::System<CameraSystem>, public entityx::Receiver<CameraSystem>
{
public:

	void update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt);
	void receive(const entityx::ComponentAddedEvent<CameraComponent>& cameraComponentAddedEvent);
	void receive(const entityx::ComponentRemovedEvent<CameraComponent>& cameraComponentRemovedEvent);
	void configure(entityx::EventManager& eventManager);

private:

	entityx::Entity m_cameraEntity;
};