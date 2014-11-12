#include "Graphics\GL\GLLightManager.h"

#include "Graphics\GL\GL.h"
#include "Graphics\GL\Core\GLShader.h"
#include "Graphics\PerspectiveCamera.h"

static const uint LIGHT_POSITION_RANGES_BINDING_POINT = 1;
static const uint LIGHT_COLORS_BINDING_POINT = 2;

#define INVALID_LIGHT_LINK 0xFFFFFFFFu
static const uint MAX_INDICES = 0xFFFFu;

void GLLightManager::initialize(uint a_maxLights)
{
	m_maxLights = a_maxLights;

	m_lightPositionRanges.reserve(m_maxLights);
	m_viewspaceLightPositionRanges.resize(m_maxLights);
	m_lightColors.reserve(m_maxLights);
}

void GLLightManager::setupShader(GLShader& a_shader)
{
	assert(a_shader.isBegun());

	m_numLightsLoc = glGetUniformLocation(a_shader.getID(), "u_numLights");

	m_lightPositionRangeBuffer.initialize(a_shader, LIGHT_POSITION_RANGES_BINDING_POINT, "LightPositionRanges", GL_STREAM_DRAW);
	m_lightColorBuffer.initialize(a_shader, LIGHT_COLORS_BINDING_POINT, "LightColors", GL_STREAM_DRAW);
}

void GLLightManager::update(const PerspectiveCamera& a_camera)
{
	uint numLights = glm::min(m_numUsedLights, m_maxLights);
	if (numLights > 0)
	{
		for (int i = 0; i < m_lightPositionRanges.size(); ++i)
		{
			m_viewspaceLightPositionRanges[i] = glm::vec4(glm::vec3(a_camera.m_viewMatrix * glm::vec4(glm::vec3(m_lightPositionRanges[i]), 1.0f)), m_lightPositionRanges[i].w);
		}

		m_lightPositionRangeBuffer.upload(sizeof(m_viewspaceLightPositionRanges[0]) * numLights, &m_viewspaceLightPositionRanges[0]);
		m_lightColorBuffer.upload(sizeof(m_lightColors[0]) * numLights, &m_lightColors[0]);
	}
	glUniform1ui(m_numLightsLoc, numLights);
}

LightHandle GLLightManager::createLight(glm::vec3 a_pos, glm::vec3 a_color, float a_radius)
{
	m_numUsedLights++;

	LightHandle handle = m_usedLightRefs.size();
	m_lightHandleIndices.push_back(m_usedLightRefs.size());
	m_usedLightRefs.push_back(m_lightPositionRanges.size());

	m_lightPositionRanges.push_back(glm::vec4(a_pos, glm::abs(a_radius)));
	m_lightColors.push_back(glm::vec4(a_color, 0.0f));

	return handle;
}

void GLLightManager::deleteLight(LightHandle a_handle)
{
	m_numUsedLights--;

	unsigned short lightIndex = m_usedLightRefs[a_handle];
	unsigned short lastLightIndex = m_lightPositionRanges.size() - 1;
	unsigned short lastHandleIndex = m_lightHandleIndices[lastLightIndex];

	rde::swap(m_usedLightRefs[a_handle], m_usedLightRefs[lastHandleIndex]);
	rde::swap(m_lightPositionRanges[lightIndex], m_lightPositionRanges[lastLightIndex]);
	rde::swap(m_lightColors[lightIndex], m_lightColors[lastLightIndex]);

	m_lightHandleIndices.pop_back();
	m_usedLightRefs.pop_back();
	m_lightPositionRanges.pop_back();
	m_lightColors.pop_back();
}

void GLLightManager::deleteLights()
{
	m_numUsedLights = 0;

	m_lightHandleIndices.clear();
	m_usedLightRefs.clear();
	m_lightPositionRanges.clear();
	m_lightColors.clear();
}

void GLLightManager::setLightPosition(LightHandle a_light, const glm::vec3& a_position)
{
	glm::vec4& posRange = m_lightPositionRanges[m_usedLightRefs[a_light]];
	posRange.x = a_position.x;
	posRange.y = a_position.y;
	posRange.z = a_position.z;
}

void GLLightManager::setLightRange(LightHandle a_light, float a_range)
{
	m_lightPositionRanges[m_usedLightRefs[a_light]].w = a_range;
}

void GLLightManager::setLightColor(LightHandle a_light, const glm::vec3& a_color)
{
	glm::vec4& col = m_lightColors[m_usedLightRefs[a_light]];
	col.r = a_color.r;
	col.g = a_color.g;
	col.b = a_color.b;
}

glm::vec3 GLLightManager::getLightPosition(LightHandle a_light)
{
	return glm::vec3(m_lightPositionRanges[m_usedLightRefs[a_light]]);
}

float GLLightManager::getLightRange(LightHandle a_light)
{
	return m_lightPositionRanges[m_usedLightRefs[a_light]].w;
}

glm::vec3 GLLightManager::getLightColor(LightHandle a_light)
{
	return glm::vec3(m_lightColors[m_usedLightRefs[a_light]]);
}