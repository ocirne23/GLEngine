#pragma once

#include "Core.h"

#include "entityx/System.h"
#include "Input/MouseListener.h"

#include <glm/glm.hpp>

class PerspectiveCamera;
struct CameraComponent;

class CameraSystem : public entityx::System<CameraSystem>, public entityx::Receiver<CameraSystem>, public MouseListener
{
public:
	CameraSystem();
	~CameraSystem();

	void update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt);
	void receive(const entityx::ComponentAddedEvent<CameraComponent>& cameraComponentAddedEvent);
	void receive(const entityx::ComponentRemovedEvent<CameraComponent>& cameraComponentRemovedEvent);
	void configure(entityx::EventManager& eventManager);

	virtual bool mouseMoved(int xPos, int yPos, int xMove, int yMove) OVERRIDE;

private:

	PerspectiveCamera* m_activeCamera = NULL;
	glm::vec3 m_lookDir;
};