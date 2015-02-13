#pragma once

#include "Core.h"

#include "entityx/System.h"
#include "Graphics/LightManager.h"

#include "rde/vector.h"

#include <glm/glm.hpp>

struct CameraComponent;
struct PointLightComponent;

class LightSystem : public entityx::System<LightSystem>, public entityx::Receiver<LightSystem>
{
public:
	enum { MAX_LIGHTS = 1024 };

	LightSystem();
	~LightSystem();

	void update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt);
	void configure(entityx::EventManager& eventManager);

	void receive(const entityx::ComponentAddedEvent<CameraComponent>& cameraComponentAddedEvent);
	void receive(const entityx::ComponentRemovedEvent<CameraComponent>& cameraComponentRemovedEvent);

	void receive(const entityx::ComponentAddedEvent<PointLightComponent>& pointLightComponentAddedEvent);
	void receive(const entityx::ComponentRemovedEvent<PointLightComponent>& pointLightComponentRemovedEvent);

	uint getNumLights();
	const glm::vec4* getViewspaceLightPositionRangeList();
	const glm::vec4* getLightColorList();

private:

	LightManager m_lightManager;

	rde::vector<entityx::Entity> m_addedPointLightEntities;

	const PerspectiveCamera* m_activeCamera				= NULL;
	const glm::vec4* m_viewspaceLightPositionRangeList	= NULL;
};