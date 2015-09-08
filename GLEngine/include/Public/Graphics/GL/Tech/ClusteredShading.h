#pragma once

#include "Core.h"
#include "3rdparty/rde/vector.h"
#include "Utils/VecForward.h"

class PerspectiveCamera;

class ClusteredShading
{
public:
	ClusteredShading() {}
	~ClusteredShading();

	void initialize(const PerspectiveCamera& camera, uint screenWidth, uint screenHeight,
	                uint pixelsPerTileW, uint pixelsPerTileH);
	void update(const PerspectiveCamera& camera, uint numLights, const glm::vec4* viewSpaceLightPositionRangeList);

	uint getGridWidth() const  { return m_gridWidth; }
	uint getGridHeight() const { return m_gridHeight; }
	uint getGridDepth() const  { return m_gridDepth; }
	uint getGridSize() const   { return m_gridSize; }

	uint getNumLightIndices() const        { return m_lightIndices.size(); }
	const glm::uvec2* getLightGrid() const { return m_lightGrid; }
	const ushort* getLightIndices() const  { return m_lightIndices.size() ? &m_lightIndices[0] : NULL; }
	float getRecLogSD1() const             { return m_recLogSD1; }
	float getRecNear() const               { return m_recNear; }

private:

	bool m_initialized    = false;
	uint m_screenWidth    = 0;
	uint m_screenHeight   = 0;
	uint m_gridWidth      = 0;
	uint m_gridHeight     = 0;
	uint m_gridDepth      = 0;
	uint m_gridSize       = 0;
	uint m_pixelsPerTileW = 0;
	uint m_pixelsPerTileH = 0;
	float m_recLogSD1     = 0.0f;
	float m_recNear       = 0.0f;

	glm::uvec2* m_lightGrid                 = NULL;
	rde::vector<ushort>* m_tileLightIndices = NULL;
	rde::vector<ushort> m_lightIndices;
};