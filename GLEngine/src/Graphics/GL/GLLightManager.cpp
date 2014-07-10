#include "Graphics\GL\GLLightManager.h"

#include "Graphics\GL\GL.h"
#include "Graphics\GL\Core\GLShader.h"

#include "Utils\SphereToSSBounds.h"

#include "Graphics\PerspectiveCamera.h"

static const unsigned int LIGHT_POSITION_RANGES_BINDING_POINT = 1;
static const unsigned int LIGHT_COLORS_BINDING_POINT = 2;
static const unsigned int LIGHT_GRID_BINDING_POINT = 3;
static const unsigned int LIGHT_INDICES_TEXTURE_INDEX = 0;
static const unsigned int LIGHT_GRID_TEXTURE_INDEX = 1;

#define INVALID_LIGHT_LINK 0xFFFFFFFFu
static const unsigned int MAX_INDICES = 0xFFFFu;

void GLLightManager::initialize(unsigned int maxLights, unsigned int pixelsPerTileW, unsigned int pixelsPerTileH, const PerspectiveCamera& camera, const Viewport& viewport)
{
	m_maxLights = maxLights;
	m_camera = &camera;
	m_viewport = viewport;

	m_pixelsPerTile.x = pixelsPerTileW;
	m_pixelsPerTile.y = pixelsPerTileH;

	m_gridWidth = viewport.width / pixelsPerTileW;
	m_gridHeight = viewport.height / pixelsPerTileH;

	uint32_t grid2dDimY = (viewport.height + pixelsPerTileH - 1) / pixelsPerTileH;
	float sD = 2.0f * glm::tan(glm::radians(camera.getVFov()) * 0.5f) / float(grid2dDimY);
	m_recLogSD1 = 1.0f / logf(sD + 1.0f);

	float zGridLocFar = logf(camera.m_far / camera.m_near) / logf(1.0f + sD);
	m_gridDepth = uint32_t(ceilf(zGridLocFar) + 0.5f);

	m_gridSize = m_gridWidth * m_gridHeight * m_gridDepth;

	m_lightGrid.resize(m_gridSize);
	m_tileLightIndices.resize(m_gridSize);

	m_lightPositionRanges.reserve(m_maxLights);
	m_viewspaceLightPositionRanges.resize(m_maxLights);
	m_lightColors.reserve(m_maxLights);
}

void GLLightManager::setupShader(GLShader& shader)
{
	assert(shader.isBegun());

	m_numLightsLoc = glGetUniformLocation(shader.getID(), "u_numLights");
	m_recLogSD1Loc = glGetUniformLocation(shader.getID(), "u_recLogSD1");
	m_recNearLoc = glGetUniformLocation(shader.getID(), "u_recNear");

	m_lightPositionRangeBuffer.initialize(shader, LIGHT_POSITION_RANGES_BINDING_POINT, "LightPositionRanges", GL_STREAM_DRAW);
	m_lightColorBuffer.initialize(shader, LIGHT_COLORS_BINDING_POINT, "LightColors", GL_STREAM_DRAW);
	m_lightGridBuffer.initialize(shader, "u_lightGrid", LIGHT_GRID_TEXTURE_INDEX, GL_RG32UI, GL_STREAM_DRAW);
	m_lightIndiceBuffer.initialize(shader, "u_lightIndices", LIGHT_INDICES_TEXTURE_INDEX, GL_R16UI, GL_STREAM_DRAW);

	glUniform1f(m_recLogSD1Loc, m_recLogSD1);
	glUniform1f(m_recNearLoc, 1.0f / m_camera->m_near);
}


void GLLightManager::update(const PerspectiveCamera& camera, float deltaSec)
{
	updateTiles(camera, deltaSec);
	unsigned int numLights = glm::min(m_numUsedLights, m_maxLights);
	if (numLights > 0)
	{
		m_lightPositionRangeBuffer.upload(sizeof(m_viewspaceLightPositionRanges[0]) * numLights, &m_viewspaceLightPositionRanges[0]);
		m_lightColorBuffer.upload(sizeof(m_lightColors[0]) * numLights, &m_lightColors[0]);
	}
	glUniform1ui(m_numLightsLoc, numLights);
}

float timePassed = 0;

void GLLightManager::updateTiles(const PerspectiveCamera& camera, float deltaSec)
{
	timePassed += deltaSec;

	memset(&m_lightGrid[0], 0, m_gridSize * sizeof(m_lightGrid[0]));
	for (auto& tileIndices : m_tileLightIndices)
		tileIndices.clear();
	m_lightIndices.clear();

	unsigned int indiceCount = 0;
	for (unsigned short i = 0; i < m_numUsedLights; ++i)
	{
		/* Lights get animated here */
		glm::vec4 offset = glm::vec4(glm::vec3(glm::sin(timePassed + i) * 9.0f), glm::cos(timePassed * 1.5f + i) * 5.0f);
		glm::vec4 lightPositionRange = m_lightPositionRanges[i] + offset;

		glm::vec3 lightPos = glm::vec3(lightPositionRange);
		float radius = glm::abs(lightPositionRange.w);

		if (!camera.m_frustum.sphereInFrustum(lightPos, radius))
			continue;

		glm::vec3 lightPosViewSpace = glm::vec3(camera.m_viewMatrix * glm::vec4(lightPos, 1.0));
		IBounds3D bounds3D = findScreenSpace3DTile(camera, lightPosViewSpace, radius, m_viewport, m_pixelsPerTile, m_recLogSD1);
		m_viewspaceLightPositionRanges[i] = glm::vec4(lightPosViewSpace, radius);

		bounds3D.minX = glm::clamp(bounds3D.minX, 0, (int) m_gridWidth);
		bounds3D.maxX = glm::clamp(bounds3D.maxX, 0, (int) m_gridWidth);
		bounds3D.minY = glm::clamp(bounds3D.minY, 0, (int) m_gridHeight);
		bounds3D.maxY = glm::clamp(bounds3D.maxY, 0, (int) m_gridHeight);
		bounds3D.minZ = glm::clamp(bounds3D.minZ, 0, (int) m_gridDepth);
		bounds3D.maxZ = glm::clamp(bounds3D.maxZ, 0, (int) m_gridDepth);
		
		for (int x = bounds3D.minX; x < bounds3D.maxX; ++x)
		{
			for (int y = bounds3D.minY; y < bounds3D.maxY; ++y)
			{
				for (int z = bounds3D.minZ; z < bounds3D.maxZ; ++z)
				{
					unsigned int gridIdx = (x * m_gridHeight + y) * m_gridDepth + z;
					m_tileLightIndices[gridIdx].push_back(i);
				}
			}
		}
	}

	for (int i = 0; i < m_tileLightIndices.size(); ++i)
	{
		int lightIndicesSize = m_lightIndices.size();
		m_lightGrid[i].begin = lightIndicesSize;
		m_lightGrid[i].end = lightIndicesSize + m_tileLightIndices[i].size();
		for (int j = 0; j < m_tileLightIndices[i].size(); ++j)
		{
			m_lightIndices.push_back(m_tileLightIndices[i][j]);
		}
	}
	if (m_lightIndices.size() > 0)
	{
		m_lightIndiceBuffer.upload(m_lightIndices.size() * sizeof(m_lightIndices[0]), &m_lightIndices[0]);
		m_lightIndiceBuffer.bind();
	}
	m_lightGridBuffer.upload(m_lightGrid.size() * sizeof(m_lightGrid[0]), &m_lightGrid[0]);
	m_lightGridBuffer.bind();
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