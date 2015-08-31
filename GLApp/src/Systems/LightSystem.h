#pragma once

#include "Core.h"

#include "3rdparty/entityx/System.h"
#include "Graphics/LightManager.h"

#include <glm/glm.hpp>

class CameraSystem;
class PointLightComponent;

class LightSystem : public entityx::System<LightSystem>, public entityx::Receiver<LightSystem>
{
public:
	enum { MAX_LIGHTS = 1024 };

	LightSystem(const CameraSystem& cameraSystem) : m_cameraSystem(cameraSystem), m_lightManager(MAX_LIGHTS) {};
	~LightSystem() {}

	void update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt) override;

	void configure(entityx::EventManager& eventManager) override;
	void receive(entityx::ComponentAddedEvent<PointLightComponent>& pointLightComponentAddedEvent);

	LightManager& getLightManager();
	uint getNumLights() const;
	const glm::vec4* getViewspaceLightPositionRangeList() const;
	const glm::vec4* getLightColorIntensityList() const;

private:

	const CameraSystem& m_cameraSystem;
	LightManager m_lightManager;
	const glm::vec4* m_viewspaceLightPositionRangeList = NULL;
};