#include "Graphics\GL\GLLightManager.h"

#include "Graphics\GL\GL.h"
#include "Graphics\GL\Core\GLShader.h"

static const unsigned int LIGHT_POSITION_RANGES_BINDING_POINT = 1;
static const unsigned int LIGHT_COLORS_BINDING_POINT = 2;

#define INVALID_LIGHT_LINK 0xFFFFFFFFu
static const unsigned int MAX_INDICES = 0xFFFFu;

void GLLightManager::initialize(unsigned int maxLights)
{
	m_maxLights = maxLights;

	m_lightPositionRanges.reserve(m_maxLights);
	m_viewspaceLightPositionRanges.resize(m_maxLights);
	m_lightColors.reserve(m_maxLights);
}

void GLLightManager::setupShader(GLShader& shader)
{
	assert(shader.isBegun());

	m_numLightsLoc = glGetUniformLocation(shader.getID(), "u_numLights");

	m_lightPositionRangeBuffer.initialize(shader, LIGHT_POSITION_RANGES_BINDING_POINT, "LightPositionRanges", GL_STREAM_DRAW);
	m_lightColorBuffer.initialize(shader, LIGHT_COLORS_BINDING_POINT, "LightColors", GL_STREAM_DRAW);
}

void GLLightManager::update()
{
	unsigned int numLights = glm::min(m_numUsedLights, m_maxLights);
	if (numLights > 0)
	{
		m_lightPositionRangeBuffer.upload(sizeof(m_viewspaceLightPositionRanges[0]) * numLights, &m_viewspaceLightPositionRanges[0]);
		m_lightColorBuffer.upload(sizeof(m_lightColors[0]) * numLights, &m_lightColors[0]);
	}
	glUniform1ui(m_numLightsLoc, numLights);
}

LightHandle GLLightManager::createLight(glm::vec3 pos, glm::vec3 color, float radius)
{
	m_numUsedLights++;

	LightHandle handle = m_usedLightRefs.size();
	m_lightHandleIndices.push_back(m_usedLightRefs.size());
	m_usedLightRefs.push_back(m_lightPositionRanges.size());

	m_lightPositionRanges.push_back(glm::vec4(pos, glm::abs(radius)));
	m_lightColors.push_back(glm::vec4(color, 0.0f));

	return handle;
}

void GLLightManager::deleteLight(LightHandle handle)
{
	m_numUsedLights--;

	unsigned short lightIndex = m_usedLightRefs[handle];
	unsigned short lastLightIndex = m_lightPositionRanges.size() - 1;
	unsigned short lastHandleIndex = m_lightHandleIndices[lastLightIndex];

	rde::swap(m_usedLightRefs[handle], m_usedLightRefs[lastHandleIndex]);
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

void GLLightManager::setLightPosition(LightHandle light, const glm::vec3& position)
{
	glm::vec4& posRange = m_lightPositionRanges[m_usedLightRefs[light]];
	posRange.x = position.x;
	posRange.y = position.y;
	posRange.z = position.z;
}

void GLLightManager::setLightRange(LightHandle light, float range)
{
	m_lightPositionRanges[m_usedLightRefs[light]].w = range;
}

void GLLightManager::setLightColor(LightHandle light, const glm::vec3& color)
{
	glm::vec4& col = m_lightColors[m_usedLightRefs[light]];
	col.r = color.r;
	col.g = color.g;
	col.b = color.b;
}

glm::vec3 GLLightManager::getLightPosition(LightHandle light)
{
	return glm::vec3(m_lightPositionRanges[m_usedLightRefs[light]]);
}

float GLLightManager::getLightRange(LightHandle light)
{
	return m_lightPositionRanges[m_usedLightRefs[light]].w;
}

glm::vec3 GLLightManager::getLightColor(LightHandle light)
{
	return glm::vec3(m_lightColors[m_usedLightRefs[light]]);
}