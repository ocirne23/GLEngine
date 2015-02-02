#pragma once

#include "Core.h"
#include "rde/vector.h"
#include "Utils/VecForward.h"
#include "Utils/Viewport.h"

#include "Graphics/GL/Core/GLConstantBuffer.h"

class PerspectiveCamera;
class GLShader;

class TiledShading
{
public:
	TiledShading() {}
	~TiledShading() {}

	void initialize(uint pixelsPerTileW, uint pixelsPerTileH, const Viewport& viewport, const PerspectiveCamera& camera);
	void setupShader(const GLShader& shader);
	void update(const PerspectiveCamera& camera, const glm::vec4* viewspaceLightPositionRangeList, uint numLights);

	uint getGridWidth() { return m_gridWidth; }
	uint getGridHeight() { return m_gridHeight; }

private:

	uint m_gridWidth;
	uint m_gridHeight;
	uint m_gridSize;

	uint m_pixelsPerTileW;
	uint m_pixelsPerTileH;

	Viewport m_viewport;

	struct LightListRef
	{
		uint begin;
		uint end;
	};
	rde::vector<LightListRef> m_lightGrid;
	rde::vector<rde::vector<ushort>> m_tileLightIndices;
	rde::vector<uint> m_lightIndices;

	GLConstantBuffer m_lightIndiceBuffer;
	GLConstantBuffer m_lightGridBuffer;
};