#pragma once

#include "Core.h"

#include "Utils/VecForward.h"

typedef uint LightHandle;

class GLShader;
class PerspectiveCamera;
class ClusteredShading;

class LightManager
{
public:
	LightManager(uint maxLights);
	~LightManager();
	LightManager(const LightManager& copy) = delete;

	LightHandle createLight();
	LightHandle createLight(const glm::vec3& pos, float radius, const glm::vec3& color, float intensity);
	void deleteLight(LightHandle light);
	void deleteLights();

	void setLight(LightHandle light, const glm::vec3& pos, float radius, const glm::vec3& color, float intensity);
	void setLightPosition(LightHandle light, const glm::vec3& position);
	void setLightRange(LightHandle light, float range);
	void setLightColor(LightHandle light, const glm::vec3& color);
	void setLightIntensity(LightHandle light, float intensity);

	const glm::vec3& getLightPosition(LightHandle light) const;
	float getLightRange(LightHandle light) const;
	const glm::vec3& getLightColor(LightHandle light) const;
	float getLightIntensity(LightHandle light) const;

	uint getNumLights() const                         { return m_numUsedLights; }
	const glm::vec4* getLightColorIntensities() const { return m_lightColorIntensities; }
	const glm::vec4* getLightPositionRanges() const   { return m_lightPositionRanges; }
	const glm::vec4* getLightColorIntensities() const { return m_lightColorIntensities; }
	const glm::vec4* getLightPositionRanges() const   { return m_lightPositionRanges; }

	const glm::vec4* updateViewspaceLightPositionRangeList(const PerspectiveCamera& a_camera);

private:

	uint m_maxLights       = 0;
	uint m_numUsedLights   = 0;
	ushort* m_lightHandles = NULL;

	glm::vec4* m_viewspaceLightPositionRanges = NULL;
	glm::vec4* m_lightPositionRanges          = NULL;
	glm::vec4* m_lightColorIntensities        = NULL;
};