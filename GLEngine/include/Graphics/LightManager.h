#pragma once

#include "Utils/Viewport.h"
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

	LightHandle createLight(const glm::vec3& pos, const glm::vec3& color, float invRadius);
	void deleteLight(LightHandle light);
	void deleteLights();

	void setLightPosition(LightHandle light, const glm::vec3& position);
	void setLightRange(LightHandle light, float range);
	void setLightColor(LightHandle light, const glm::vec3& color);

	const glm::vec3& getLightPosition(LightHandle light) const;
	float getLightRange(LightHandle light) const;
	const glm::vec3& getLightColor(LightHandle light) const;

	uint getNumLights() const						{ return m_numUsedLights; }
	const glm::vec4* getLightPositionRanges() const	{ return m_lightPositionRanges; }
	const glm::vec4* getLightColors() const 		{ return m_lightColors; }

private:

	uint m_maxLights;
	uint m_numUsedLights;

	glm::vec4* m_lightPositionRanges;
	glm::vec4* m_lightColors;

	ushort* m_usedLightRefs;
	ushort* m_lightHandleIndices;
};