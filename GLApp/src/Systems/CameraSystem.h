#pragma once

#include "Core.h"

#include "3rdparty/entityx/System.h"

class PerspectiveCamera;
struct CameraComponent;

class CameraSystem : public entityx::System<CameraSystem>, public entityx::Receiver<CameraSystem>
{
public:

	void update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt) override;

	void configure(entityx::EventManager& eventManager) override;
	void receive(entityx::ComponentAddedEvent<CameraComponent>& cameraComponentAddedEvent);

	const PerspectiveCamera* getActiveCamera() const;

private:

	entityx::Entity m_cameraEntity;
	const PerspectiveCamera* m_activeCamera;
};