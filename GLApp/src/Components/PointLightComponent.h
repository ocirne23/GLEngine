#pragma once

#include "Core.h"

#include <glm/glm.hpp>

class LightSystem;
class LightManager;

class PointLightComponent
{
	friend class LightSystem;
public:

	~PointLightComponent();

	void set(const glm::vec3& position, float radius, const glm::vec3& color, float intensity);
	void setPosition(const glm::vec3& position);
	void setRadius(float radius);
	void setColor(const glm::vec3& color);
	void setIntensity(float intensity);

	const glm::vec3& getPosition() const;
	float getRadius() const;
	const glm::vec3& getColor() const;
	float getIntensity() const;

private:

	LightManager* m_lightManager = NULL;
	uint m_lightHandle           = 0;
};