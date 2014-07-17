#pragma once

#include "rde\vector.h"
#include "Graphics\GL\GLTypes.h"
#include "Graphics\GL\Core\GLConstantBuffer.h"
#include "Graphics\GL\Core\GLTextureBuffer.h"
#include "Utils\Viewport.h"

#include <glm\glm.hpp>

typedef unsigned int LightHandle;

class GLShader;
class PerspectiveCamera;
class ClusteredShading;

class GLLightManager
{
public:
	GLLightManager() : m_numUsedLights(0), m_maxLights(0), m_numLightsLoc(0)
	{};
	~GLLightManager() {};

	void initialize(unsigned int m_maxLights);
	void setupShader(GLShader& shader);
	void update(const PerspectiveCamera& camera);

	unsigned int getNumLights() { return m_numUsedLights; }
	const glm::vec4* getViewspaceLightPositionRangeListBegin()
	{ 
		return m_viewspaceLightPositionRanges.size() ? &m_viewspaceLightPositionRanges[0] : NULL;
	}

	LightHandle createLight(glm::vec3 pos, glm::vec3 color, float invRadius);
	void deleteLight(LightHandle light);
	void deleteLights();

	void setLightPosition(LightHandle light, const glm::vec3& position);
	void setLightRange(LightHandle light, float range);
	void setLightColor(LightHandle light, const glm::vec3& color);
	glm::vec3 getLightPosition(LightHandle light);
	float getLightRange(LightHandle light);
	glm::vec3 getLightColor(LightHandle light);

private:

	static const unsigned int AVAILABLE_HANDLE = 0xFFFFFFFF;

	unsigned int m_maxLights;
	unsigned int m_numUsedLights;

	rde::vector<glm::vec4> m_lightPositionRanges;
	rde::vector<glm::vec4> m_viewspaceLightPositionRanges;
	rde::vector<glm::vec4> m_lightColors;

	rde::vector<unsigned short> m_usedLightRefs;
	rde::vector<unsigned short> m_lightHandleIndices;

	GLint m_numLightsLoc;
	
	GLConstantBuffer m_lightPositionRangeBuffer;
	GLConstantBuffer m_lightColorBuffer;
};