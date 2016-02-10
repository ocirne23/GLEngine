#include "Graphics/Utils/LightManager.h"

#include "Graphics/Utils/PerspectiveCamera.h"
#include "EASTL/algorithm.h"

LightHandle LightManager::INVALID_HANDLE = 0xFFFF;

LightManager::LightManager(uint a_maxLights) : m_maxLights(a_maxLights)
{
	m_lightPositionRanges = new glm::vec4[m_maxLights];
	m_lightColorIntensities = new glm::vec4[m_maxLights];
	m_lightHandles = new ushort[m_maxLights];
}

LightManager::~LightManager()
{
	SAFE_DELETE_ARRAY(m_lightPositionRanges);
	SAFE_DELETE_ARRAY(m_lightColorIntensities);
	SAFE_DELETE_ARRAY(m_lightHandles);
}

LightHandle LightManager::createLight()
{
	return createLight(glm::vec3(0), 0.0f, glm::vec3(0), 0.0f);
}

LightHandle LightManager::createLight(const glm::vec3& a_pos, float a_radius, const glm::vec3& a_color, float a_intensity)
{
	if (m_numUsedLights < m_maxLights)
	{
		m_lightHandles[m_numUsedLights] = m_numUsedLights;
		m_lightPositionRanges[m_numUsedLights] = glm::vec4(a_pos, a_radius);
		m_lightColorIntensities[m_numUsedLights] = glm::vec4(glm::normalize(a_color), a_intensity);
		return m_numUsedLights++;
	}
	return INVALID_HANDLE;
}

void LightManager::deleteLight(LightHandle a_handle)
{
	assert(m_numUsedLights > 0);
	if (a_handle == INVALID_HANDLE)
		return;
	--m_numUsedLights;
	if (a_handle != m_numUsedLights)
	{
		const ushort last = m_lightHandles[m_numUsedLights];
		const ushort idx = m_lightHandles[a_handle];
		eastl::swap(m_lightPositionRanges[last], m_lightPositionRanges[idx]);
		eastl::swap(m_lightColorIntensities[last], m_lightColorIntensities[idx]);
	}
}

void LightManager::deleteLights()
{
	m_numUsedLights = 0;
}

void LightManager::setLight(LightHandle a_light, const glm::vec3& a_pos, float a_radius, const glm::vec3& a_color, float a_intensity)
{
	assert(a_light < m_numUsedLights);
	const ushort idx = m_lightHandles[a_light];
	m_lightPositionRanges[idx] = glm::vec4(a_pos, a_radius);
	m_lightColorIntensities[idx] = glm::vec4(a_color, a_intensity);
}

void LightManager::setLightPosition(LightHandle a_light, const glm::vec3& a_position)
{
	assert(a_light < m_numUsedLights);
	glm::vec4& posRange = m_lightPositionRanges[m_lightHandles[a_light]];
	posRange.x = a_position.x;
	posRange.y = a_position.y;
	posRange.z = a_position.z;
}

void LightManager::setLightRange(LightHandle a_light, float a_range)
{
	assert(a_light < m_numUsedLights);
	m_lightPositionRanges[m_lightHandles[a_light]].w = a_range;
}

void LightManager::setLightColor(LightHandle a_light, const glm::vec3& a_color)
{
	assert(a_light < m_numUsedLights);
	glm::vec4& col = m_lightColorIntensities[m_lightHandles[a_light]];
	col.r = a_color.r;
	col.g = a_color.g;
	col.b = a_color.b;
}

void LightManager::setLightIntensity(LightHandle a_light, float a_intensity)
{
	assert(a_light < m_numUsedLights);
	m_lightColorIntensities[m_lightHandles[a_light]].a = a_intensity;
}

const glm::vec3& LightManager::getLightPosition(LightHandle a_light) const
{
	assert(a_light < m_numUsedLights);
	return rcast<glm::vec3&>(m_lightPositionRanges[m_lightHandles[a_light]]);
}

float LightManager::getLightRange(LightHandle a_light) const
{
	assert(a_light < m_numUsedLights);
	return m_lightPositionRanges[m_lightHandles[a_light]].w;
}

const glm::vec3& LightManager::getLightColor(LightHandle a_light) const
{
	assert(a_light < m_numUsedLights);
	return rcast<glm::vec3&>(m_lightColorIntensities[m_lightHandles[a_light]]);
}

float LightManager::getLightIntensity(LightHandle a_light) const
{
	assert(a_light < m_numUsedLights);
	return m_lightColorIntensities[m_lightHandles[a_light]].a;
}