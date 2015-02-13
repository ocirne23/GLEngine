#include "Graphics/LightManager.h"

#include "Graphics/PerspectiveCamera.h"
#include "Graphics/GL/GLMesh.h"
#include "rde/algorithm.h"

#include <glm/glm.hpp>

LightManager::LightManager(uint a_maxLights) : m_maxLights(a_maxLights), m_numUsedLights(0)
{
	m_viewspaceLightPositionRanges = new glm::vec4[m_maxLights];
	m_lightPositionRanges = new glm::vec4[m_maxLights];
	m_lightColors = new glm::vec4[m_maxLights];

	m_lightHandles = new ushort[m_maxLights];
}

LightManager::~LightManager()
{
	delete[] m_viewspaceLightPositionRanges;
	delete[] m_lightPositionRanges;
	delete[] m_lightColors;
	delete[] m_lightHandles;
}

LightHandle LightManager::createLight(const glm::vec3& a_pos, const glm::vec3& a_color, float a_radius)
{
	assert(m_numUsedLights < m_maxLights - 1);
	m_lightHandles[m_numUsedLights] = m_numUsedLights;

	m_lightPositionRanges[m_numUsedLights] = glm::vec4(a_pos, a_radius);
	m_lightColors[m_numUsedLights] = glm::vec4(a_color, 0.0f);

	return m_numUsedLights++;
}

void LightManager::deleteLight(LightHandle a_handle)
{
	assert(m_numUsedLights);
	--m_numUsedLights;
	ushort last = m_lightHandles[m_numUsedLights];
	rde::swap(m_lightPositionRanges[last], m_lightPositionRanges[m_lightHandles[a_handle]]);
	rde::swap(m_lightColors[last], m_lightColors[m_lightHandles[a_handle]]);
	m_lightHandles[a_handle] = m_maxLights;
}

void LightManager::deleteLights()
{
	m_numUsedLights = 0;
}

void LightManager::setLightPosition(LightHandle a_light, const glm::vec3& a_position)
{
	glm::vec4& posRange = m_lightPositionRanges[m_lightHandles[a_light]];
	posRange.x = a_position.x;
	posRange.y = a_position.y;
	posRange.z = a_position.z;
}

void LightManager::setLightRange(LightHandle a_light, float a_range)
{
	m_lightPositionRanges[m_lightHandles[a_light]].w = a_range;
}

void LightManager::setLightColor(LightHandle a_light, const glm::vec3& a_color)
{
	glm::vec4& col = m_lightColors[m_lightHandles[a_light]];
	col.r = a_color.r;
	col.g = a_color.g;
	col.b = a_color.b;
}

const glm::vec3& LightManager::getLightPosition(LightHandle a_light) const
{
	return (glm::vec3&) m_lightPositionRanges[m_lightHandles[a_light]];
}

float LightManager::getLightRange(LightHandle a_light) const
{
	return m_lightPositionRanges[m_lightHandles[a_light]].w;
}

const glm::vec3& LightManager::getLightColor(LightHandle a_light) const
{
	return (glm::vec3&) m_lightColors[m_lightHandles[a_light]];
}

const glm::vec4* LightManager::updateViewspaceLightPositionRangeList(const PerspectiveCamera& a_camera)
{
	for (uint i = 0; i < m_numUsedLights; ++i)
	{
		m_viewspaceLightPositionRanges[i] = glm::vec4(a_camera.m_viewMatrix * glm::vec4(glm::vec3(m_lightPositionRanges[i]), 1.0));
		m_viewspaceLightPositionRanges[i].w = m_lightPositionRanges[i].w;
	}
	return m_numUsedLights ? &m_viewspaceLightPositionRanges[0] : NULL;
}
