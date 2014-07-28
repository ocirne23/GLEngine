#include "Graphics\GL\Tech\TiledShading.h"
#include "Graphics\GL\GL.h"
#include "Graphics\GL\Core\GLShader.h"
#include "Utils\sphereToScreenSpaceBounds.h"
#include "Graphics\PerspectiveCamera.h"

#include <glm\glm.hpp>

#include <assert.h>

void TiledShading::initialize(uint pixelsPerTileW, uint pixelsPerTileH, const Viewport& viewport, const PerspectiveCamera& camera)
{
	m_viewport = viewport;

	m_pixelsPerTileW = pixelsPerTileW;
	m_pixelsPerTileH = pixelsPerTileH;

	m_gridWidth = viewport.width / pixelsPerTileW;
	m_gridHeight = viewport.height / pixelsPerTileH;

	m_gridSize = m_gridWidth * m_gridHeight;

	m_lightGrid.resize(m_gridSize);
	m_tileLightIndices.resize(m_gridSize);
}

void TiledShading::setupShader(const GLShader& shader)
{
	assert(shader.isBegun());

	m_lightGridBuffer.initialize(shader, 0, "LightGrid", GL_STREAM_DRAW);
	m_lightIndiceBuffer.initialize(shader, 1, "LightIndices", GL_STREAM_DRAW);
}

void TiledShading::update(const PerspectiveCamera& camera, const glm::vec4* viewspaceLightPositionRangeList, uint numLights)
{
	memset(&m_lightGrid[0], 0, m_gridSize * sizeof(m_lightGrid[0]));
	for (auto& tileIndices : m_tileLightIndices)
		tileIndices.clear();
	m_lightIndices.clear();

	for (unsigned short i = 0; i < numLights; ++i)
	{
		glm::vec4 lightPositionRange = viewspaceLightPositionRangeList[i];
		float radius = lightPositionRange.w;
		glm::vec3 lightPosition = glm::vec3(lightPositionRange);

		IBounds2D bounds2D = sphereToScreenSpaceBounds2D(camera, lightPosition, radius, m_viewport);

		bounds2D.minX = glm::clamp(bounds2D.minX, 0, (int) m_gridWidth);
		bounds2D.maxX = glm::clamp(bounds2D.maxX, 0, (int) m_gridWidth);
		bounds2D.minY = glm::clamp(bounds2D.minY, 0, (int) m_gridHeight);
		bounds2D.maxY = glm::clamp(bounds2D.maxY, 0, (int) m_gridHeight);

		for (int x = bounds2D.minX; x < bounds2D.maxX; ++x)
		{
			for (int y = bounds2D.minY; y < bounds2D.maxY; ++y)
			{
				unsigned int gridIdx = x * m_gridHeight + y;
				m_tileLightIndices[gridIdx].push_back(i);
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