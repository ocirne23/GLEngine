#pragma once

#include "Core.h"

#include "Graphics/GL/GLTypes.h"
#include "rde/vector.h"
#include "Utils/VecForward.h"
#include "Utils/Viewport.h"

class PerspectiveCamera;
class GLShader;

class ClusteredShading
{
public:
	ClusteredShading() {}
	~ClusteredShading() {}

	void initialize(uint pixelsPerTileW, uint pixelsPerTileH, const Viewport& viewport, const PerspectiveCamera& camera);
	void update(const PerspectiveCamera& camera, uint numLights, const glm::vec4* lightPositionRangeList);

	uint getGridWidth() const				{ return m_gridWidth; }
	uint getGridHeight() const				{ return m_gridHeight; }
	uint getGridDepth() const				{ return m_gridDepth; }
	uint getGridSize() const					{ return m_gridSize; }

	uint getNumLightIndices() const			{ return m_lightIndices.size(); }

	const glm::uvec2* getLightGrid() const	{ return m_lightGrid; }
	const ushort* getLightIndices() const	{ return m_lightIndices.size() ? &m_lightIndices[0] : NULL; }
	float getRecLogSD1() const				{ return m_recLogSD1; }
	float getRecNear() const					{ return m_recNear; }

private:

	float m_recLogSD1;
	float m_recNear;

	uint m_gridWidth;
	uint m_gridHeight;
	uint m_gridDepth;
	uint m_gridSize;

	uint m_pixelsPerTileW;
	uint m_pixelsPerTileH;

	Viewport m_viewport;

	glm::uvec2* m_lightGrid;
	rde::vector<ushort>* m_tileLightIndices;
	rde::vector<ushort> m_lightIndices;
};