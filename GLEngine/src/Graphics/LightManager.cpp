#include "Graphics/LightManager.h"

#include "Graphics/GL/GLMesh.h"
#include "rde/algorithm.h"
#include <glm/glm.hpp>

LightManager::LightManager(uint a_maxLights) : m_maxLights(a_maxLights), m_numUsedLights(0)
{
	m_lightPositionRanges = new glm::vec4[m_maxLights];
	m_lightColors = new glm::vec4[m_maxLights];

	m_usedLightRefs = new ushort[m_maxLights];
	m_lightHandleIndices = new ushort[m_maxLights];
}

LightManager::~LightManager()
{
	delete[] m_lightPositionRanges;
	delete[] m_lightColors;
	
	delete[] m_usedLightRefs;
	delete[] m_lightHandleIndices;
}

LightHandle LightManager::createLight(const glm::vec3& a_pos, const glm::vec3& a_color, float a_radius)
{
	m_lightHandleIndices[m_numUsedLights] = m_numUsedLights;
	m_usedLightRefs[m_numUsedLights] = m_numUsedLights;

	m_lightPositionRanges[m_numUsedLights] = glm::vec4(a_pos, a_radius);
	m_lightColors[m_numUsedLights] = glm::vec4(a_color, 0.0f);

	return m_numUsedLights++;
}

void LightManager::deleteLight(LightHandle a_handle)
{
	m_numUsedLights--;

	ushort lightIndex = m_usedLightRefs[a_handle];
	ushort lastLightIndex = m_numUsedLights - 1;
	ushort lastHandleIndex = m_lightHandleIndices[lastLightIndex];

	rde::swap(m_usedLightRefs[a_handle], m_usedLightRefs[lastHandleIndex]);
	rde::swap(m_lightPositionRanges[lightIndex], m_lightPositionRanges[lastLightIndex]);
	rde::swap(m_lightColors[lightIndex], m_lightColors[lastLightIndex]);
}

void LightManager::deleteLights()
{
	m_numUsedLights = 0;
}

void LightManager::setLightPosition(LightHandle a_light, const glm::vec3& a_position)
{
	glm::vec4& posRange = m_lightPositionRanges[m_usedLightRefs[a_light]];
	posRange.x = a_position.x;
	posRange.y = a_position.y;
	posRange.z = a_position.z;
}

void LightManager::setLightRange(LightHandle a_light, float a_range)
{
	m_lightPositionRanges[m_usedLightRefs[a_light]].w = a_range;
}

void LightManager::setLightColor(LightHandle a_light, const glm::vec3& a_color)
{
	glm::vec4& col = m_lightColors[m_usedLightRefs[a_light]];
	col.r = a_color.r;
	col.g = a_color.g;
	col.b = a_color.b;
}

const glm::vec3& LightManager::getLightPosition(LightHandle a_light) const
{
	return (glm::vec3&) m_lightPositionRanges[m_usedLightRefs[a_light]];
}

float LightManager::getLightRange(LightHandle a_light) const
{
	return m_lightPositionRanges[m_usedLightRefs[a_light]].w;
}

const glm::vec3& LightManager::getLightColor(LightHandle a_light) const
{
	return (glm::vec3&) m_lightColors[m_usedLightRefs[a_light]];
}