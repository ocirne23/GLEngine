#pragma once

#include "Core.h"
#include "Utils/VecForward.h"
#include "eastl/uvector.h"

class PerspectiveCamera;

class TiledShading
{
public:
	TiledShading() {}
	~TiledShading();

	void initialize(uint pixelsPerTileW, uint pixelsPerTileH, uint screenWidth, uint screenHeight, const PerspectiveCamera& camera);
	void update(const PerspectiveCamera& camera, uint numLights, const glm::vec4* viewspaceLightPositionRangeList);

	uint getGridWidth() const  { return m_gridWidth; }
	uint getGridHeight() const { return m_gridHeight; }
	uint getGridSize() const   { return m_gridSize; }

	uint getNumLightIndices() const        { return m_lightIndices.size(); }
	const glm::uvec2* getLightGrid() const { return m_lightGrid; }
	const ushort* getLightIndices() const  { return m_lightIndices.size() ? &m_lightIndices[0] : NULL; }

private:

	bool m_initialized    = false;
	uint m_screenWidth    = 0;
	uint m_screenHeight   = 0;
	uint m_gridWidth      = 0;
	uint m_gridHeight     = 0;
	uint m_gridSize       = 0;
	uint m_pixelsPerTileW = 0;
	uint m_pixelsPerTileH = 0;

	glm::uvec2* m_lightGrid                 = NULL;
	eastl::vector<ushort>* m_tileLightIndices = NULL;
	eastl::vector<ushort> m_lightIndices;
};