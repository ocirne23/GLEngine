#include "Graphics\GL\ClusteredShading.h"

#include "Utils\Viewport.h"
#include "Graphics\PerspectiveCamera.h"
#include "Graphics\GL\GL.h"
#include "Graphics\GL\Core\GLShader.h"
#include "Utils\Bounds3D.h"
#include "Utils\SphereToSSBounds.h"

#include <assert.h>

#include <glm\glm.hpp>

void ClusteredShading::initialize(uint pixelsPerTileW, uint pixelsPerTileH, const Viewport& viewport, 
	const PerspectiveCamera& camera)
{
	m_viewport = viewport;

	m_pixelsPerTileW = pixelsPerTileW;
	m_pixelsPerTileH = pixelsPerTileH;

	m_gridWidth = viewport.width / pixelsPerTileW;
	m_gridHeight = viewport.height / pixelsPerTileH;

	uint grid2dDimY = (viewport.height + pixelsPerTileH - 1) / pixelsPerTileH;
	float sD = 2.0f * glm::tan(glm::radians(camera.getVFov()) * 0.5f) / float(grid2dDimY);
	m_recLogSD1 = 1.0f / logf(sD + 1.0f);
	m_recNear = 1.0f / camera.m_near;
	float zGridLocFar = logf(camera.m_far / camera.m_near) / logf(1.0f + sD);

	m_gridDepth = uint(ceilf(zGridLocFar) + 0.5f);
	m_gridSize = m_gridWidth * m_gridHeight * m_gridDepth;

	m_lightGrid.resize(m_gridSize);
	m_tileLightIndices.resize(m_gridSize);
}

void ClusteredShading::setupShader(const GLShader& shader, uint gridTextureIdx, uint lightIdTextureIdx)
{
	assert(shader.isBegun());

	m_lightGridBuffer.initialize(shader, "u_lightGrid", gridTextureIdx, GL_RG32UI, GL_STREAM_DRAW);
	m_lightIndiceBuffer.initialize(shader, "u_lightIndices", lightIdTextureIdx, GL_R16UI, GL_STREAM_DRAW);

	m_recLogSD1Loc = glGetUniformLocation(shader.getID(), "u_recLogSD1");
	m_recNearLoc = glGetUniformLocation(shader.getID(), "u_recNear");

	glUniform1f(m_recLogSD1Loc, m_recLogSD1);
	glUniform1f(m_recNearLoc, m_recNear);
}

void ClusteredShading::update(const PerspectiveCamera& camera, const glm::vec4* lightPositionRangeList, uint numLights)
{
	memset(&m_lightGrid[0], 0, m_gridSize * sizeof(m_lightGrid[0]));
	for (auto& tileIndices : m_tileLightIndices)
		tileIndices.clear();
	m_lightIndices.clear();

	for (unsigned short i = 0; i < numLights; ++i)
	{
		glm::vec4 lightPositionRange = lightPositionRangeList[i];
		float radius = lightPositionRange.w;
		lightPositionRange.w = 1.0f;
		glm::vec3 lightPositionViewSpace = glm::vec3(camera.m_viewMatrix * lightPositionRange);

		IBounds3D bounds3D = findScreenSpace3DTile(camera, lightPositionViewSpace, radius, m_viewport, m_pixelsPerTileW, m_pixelsPerTileH, m_recLogSD1);

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
