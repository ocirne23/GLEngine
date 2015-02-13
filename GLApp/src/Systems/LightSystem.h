#pragma once

#include "Core.h"

#include "entityx/System.h"
#include "Graphics/LightManager.h"
#include "Input/KeyListener.h"

#include <glm/glm.hpp>

struct CameraComponent;

class LightSystem : public entityx::System<LightSystem>, public entityx::Receiver<LightSystem>, public KeyListener
{
public:
	enum { MAX_LIGHTS = 1024 };

	LightSystem();
	~LightSystem();

	void update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt);
	void receive(const entityx::ComponentAddedEvent<CameraComponent>& cameraComponentAddedEvent);
	void receive(const entityx::ComponentRemovedEvent<CameraComponent>& cameraComponentRemovedEvent);
	void configure(entityx::EventManager& eventManager);

	uint getNumLights();
	const glm::vec4* getViewspaceLightPositionRangeList();
	const glm::vec4* getLightColorList();

	virtual bool keyDown(Key key) OVERRIDE;

private:

	LightManager m_lightManager;

	const PerspectiveCamera* m_activeCamera				= NULL;
	const glm::vec4* m_viewspaceLightPositionRangeList	= NULL;
};