#include "Graphics/GL/Tech/TiledShading.h"

#include "Graphics/PerspectiveCamera.h"
#include "Graphics/GL/GL.h"
#include "Graphics/GL/Wrappers/GLShader.h"
#include "Utils/sphereToScreenSpaceBounds.h"

#include <assert.h>
#include <glm/glm.hpp>

TiledShading::~TiledShading()
{
	SAFE_DELETE_ARRAY(m_lightGrid);
	SAFE_DELETE_ARRAY(m_tileLightIndices);
}

void TiledShading::initialize(uint a_pixelsPerTileW, uint a_pixelsPerTileH, uint a_screenWidth, uint a_screenHeight, const PerspectiveCamera& a_camera)
{
	SAFE_DELETE_ARRAY(m_lightGrid);
	SAFE_DELETE_ARRAY(m_tileLightIndices);

	m_screenWidth = a_screenWidth;
	m_screenHeight = a_screenHeight;
	m_pixelsPerTileW = a_pixelsPerTileW;
	m_pixelsPerTileH = a_pixelsPerTileH;

	m_gridWidth = a_screenWidth / a_pixelsPerTileW;
	m_gridHeight = a_screenHeight / a_pixelsPerTileH;

	m_gridSize = m_gridWidth * m_gridHeight;

	m_lightGrid = new glm::uvec2[m_gridSize];
	m_tileLightIndices = new eastl::vector<ushort>[m_gridSize];

	m_initialized = true;
}

void TiledShading::update(const PerspectiveCamera& a_camera, uint a_numLights, const glm::vec4* a_viewspaceLightPositionRangeList)
{
	assert(m_initialized);

	memset(&m_lightGrid[0], 0, m_gridSize * sizeof(m_lightGrid[0]));
	for (uint i = 0; i < m_gridSize; ++i)
		m_tileLightIndices[i].clear();
	m_lightIndices.clear();

	for (ushort i = 0; i < a_numLights; ++i)
	{
		glm::vec4 lightPositionRange = a_viewspaceLightPositionRangeList[i];
		float radius = lightPositionRange.w;
		glm::vec3 lightPosition = glm::vec3(lightPositionRange);

		IBounds2D bounds2D = sphereToScreenSpaceBounds2D(a_camera, lightPosition, radius, m_screenWidth, m_screenHeight);

		bounds2D.minX = glm::clamp(bounds2D.minX, 0, (int) m_gridWidth);
		bounds2D.maxX = glm::clamp(bounds2D.maxX, 0, (int) m_gridWidth);
		bounds2D.minY = glm::clamp(bounds2D.minY, 0, (int) m_gridHeight);
		bounds2D.maxY = glm::clamp(bounds2D.maxY, 0, (int) m_gridHeight);

		for (int x = bounds2D.minX; x < bounds2D.maxX; ++x)
		{
			for (int y = bounds2D.minY; y < bounds2D.maxY; ++y)
			{
				uint gridIdx = x * m_gridHeight + y;
				m_tileLightIndices[gridIdx].push_back(i);
			}
		}
	}

	for (uint i = 0; i < m_gridSize; ++i)
	{
		int lightIndicesSize = m_lightIndices.size();
		m_lightGrid[i].x = lightIndicesSize;
		m_lightGrid[i].y = lightIndicesSize + m_tileLightIndices[i].size();
		for (uint j = 0; j < m_tileLightIndices[i].size(); ++j)
			m_lightIndices.push_back(m_tileLightIndices[i][j]);
	}
}
