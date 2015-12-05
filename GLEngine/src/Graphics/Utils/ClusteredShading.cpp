#include "Graphics/Utils/ClusteredShading.h"

#include "Graphics/Utils/PerspectiveCamera.h"
#include "Graphics/GL/GL.h"
#include "Graphics/GL/Wrappers/GLShader.h"
#include "Graphics/Utils/ClusteredTiledShadingUtils.h"

#include <assert.h>
#include <glm/glm.hpp>

ClusteredShading::~ClusteredShading()
{
	SAFE_DELETE_ARRAY(m_lightGrid);
	SAFE_DELETE_ARRAY(m_tileLightIndices);
}

void ClusteredShading::initialize(const PerspectiveCamera& a_camera, uint a_screenWidth, uint a_screenHeight, 
	                              uint a_pixelsPerTileW, uint a_pixelsPerTileH)
{
	SAFE_DELETE_ARRAY(m_lightGrid);
	SAFE_DELETE_ARRAY(m_tileLightIndices);

	m_screenWidth = a_screenWidth;
	m_screenHeight = a_screenHeight;
	m_pixelsPerTileW = a_pixelsPerTileW;
	m_pixelsPerTileH = a_pixelsPerTileH;

	m_gridWidth = a_screenWidth / a_pixelsPerTileW + 1;
	m_gridHeight = a_screenHeight / a_pixelsPerTileH + 1;

	m_recNear = 1.0f / a_camera.getNear();

	const uint grid2dDimY = (a_screenHeight + a_pixelsPerTileH - 1) / a_pixelsPerTileH;
	const float sD = 2.0f * glm::tan(glm::radians(a_camera.getVFov()) * 0.5f) / float(grid2dDimY);
	m_recLogSD1 = 1.0f / logf(sD + 1.0f);

	const float zGridLocFar = logf(a_camera.getFar() / a_camera.getNear()) / logf(1.0f + sD);
	m_gridDepth = uint(ceilf(zGridLocFar) + 0.5f);
	m_gridSize = m_gridWidth * m_gridHeight * m_gridDepth;

	m_lightGrid = new glm::uvec2[m_gridSize];
	m_tileLightIndices = new eastl::vector<ushort>[m_gridSize];

	m_initialized = true;
}

void ClusteredShading::update(const PerspectiveCamera& a_camera, uint a_numLights, const glm::vec4* a_viewSpaceLightPositionRangeList)
{
	assert(m_initialized);

	memset(&m_lightGrid[0], 0, m_gridSize * sizeof(m_lightGrid[0]));
	for (uint i = 0; i < m_gridSize; ++i)
		m_tileLightIndices[i].clear();
	m_lightIndices.clear();

	for (ushort i = 0; i < a_numLights; ++i)
	{
		const glm::vec4 lightPositionRange = a_viewSpaceLightPositionRangeList[i];
		const float radius = lightPositionRange.w;
		const glm::vec3 lightPosition(lightPositionRange);

		IBounds3D bounds3D = ClusteredTiledShadingUtils::sphereToScreenSpaceBounds3D(a_camera, lightPosition, radius, m_screenWidth, m_screenHeight, m_pixelsPerTileW, m_pixelsPerTileH, m_recLogSD1);

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
					const uint gridIdx = (x * m_gridHeight + y) * m_gridDepth + z;
					m_tileLightIndices[gridIdx].push_back(i);
				}
			}
		}
	}

	for (uint i = 0; i < m_gridSize; ++i)
	{
		const int lightIndicesSize = m_lightIndices.size();
		m_lightGrid[i].x = lightIndicesSize;
		m_lightGrid[i].y = lightIndicesSize + m_tileLightIndices[i].size();
		for (uint j = 0; j < m_tileLightIndices[i].size(); ++j)
			m_lightIndices.push_back(m_tileLightIndices[i][j]);
	}
}