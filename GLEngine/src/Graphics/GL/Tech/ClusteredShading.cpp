#include "Graphics/GL/Tech/ClusteredShading.h"

#include "Utils/Viewport.h"
#include "Graphics/PerspectiveCamera.h"
#include "Graphics/GL/GL.h"
#include "Graphics/GL/Core/GLShader.h"
#include "Utils/Bounds3D.h"
#include "Utils/sphereToScreenSpaceBounds.h"

#include <assert.h>

#include <glm/glm.hpp>

void ClusteredShading::initialize(uint a_pixelsPerTileW, uint a_pixelsPerTileH, const Viewport& a_viewport,
	const PerspectiveCamera& a_camera)
{
	m_viewport = a_viewport;

	m_pixelsPerTileW = a_pixelsPerTileW;
	m_pixelsPerTileH = a_pixelsPerTileH;

	m_gridWidth = a_viewport.width / a_pixelsPerTileW;
	m_gridHeight = a_viewport.height / a_pixelsPerTileH;

	uint grid2dDimY = (a_viewport.height + a_pixelsPerTileH - 1) / a_pixelsPerTileH;
	float sD = 2.0f * glm::tan(glm::radians(a_camera.getVFov()) * 0.5f) / float(grid2dDimY);
	m_recLogSD1 = 1.0f / logf(sD + 1.0f);
	m_recNear = 1.0f / a_camera.m_near;
	float zGridLocFar = logf(a_camera.m_far / a_camera.m_near) / logf(1.0f + sD);

	m_gridDepth = uint(ceilf(zGridLocFar) + 0.5f);
	m_gridSize = m_gridWidth * m_gridHeight * m_gridDepth;

	m_lightGrid = new glm::uvec2[m_gridSize];
	m_tileLightIndices = new rde::vector<ushort>[m_gridSize];
}

void ClusteredShading::update(const PerspectiveCamera& a_camera, const glm::vec4* a_lightPositionRangeList, uint a_numLights)
{
	memset(&m_lightGrid[0], 0, m_gridSize * sizeof(m_lightGrid[0]));

	for (uint i = 0; i < m_gridSize; ++i)
	{
		m_tileLightIndices[i].clear();
	}
	m_lightIndices.clear();

	for (ushort i = 0; i < a_numLights; ++i)
	{
		glm::vec4 lightPositionRange = a_lightPositionRangeList[i];
		float radius = lightPositionRange.w;
		glm::vec3 lightPosition(a_camera.m_viewMatrix * glm::vec4(glm::vec3(lightPositionRange), 1.0f));
		print("LightPosition: %f %f %f \n", lightPositionRange.x, lightPositionRange.y, lightPosition.z);

		IBounds3D bounds3D = sphereToScreenSpaceBounds3D(a_camera, lightPosition, radius, m_viewport, m_pixelsPerTileW, m_pixelsPerTileH, m_recLogSD1);
		//print("Light: %i %i %i %i %i %i \n", bounds3D.minX, bounds3D.maxX, bounds3D.minY, bounds3D.maxY, bounds3D.minZ, bounds3D.maxZ);

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
					uint gridIdx = (x * m_gridHeight + y) * m_gridDepth + z;
					m_tileLightIndices[gridIdx].push_back(i);
				}
			}
		}
	}

	for (uint i = 0; i < m_gridSize; ++i)
	{
		int lightIndicesSize = m_lightIndices.size();
		m_lightGrid[i].x = lightIndicesSize;
		m_lightGrid[i].y = lightIndicesSize + m_tileLightIndices[i].size();
		for (int j = 0; j < m_tileLightIndices[i].size(); ++j)
		{
			m_lightIndices.push_back(m_tileLightIndices[i][j]);
		}
	}
}