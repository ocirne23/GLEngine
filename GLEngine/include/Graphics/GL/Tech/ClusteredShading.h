#pragma once

#include "Core.h"
#include "rde/vector.h"
#include "Utils/VecForward.h"
#include "Utils/Viewport.h"

#include "Graphics/GL/Core/GLTextureBuffer.h"

class PerspectiveCamera;
class GLShader;

class ClusteredShading
{
public:
	ClusteredShading() {};
	~ClusteredShading() {};

	void initialize(uint pixelsPerTileW, uint pixelsPerTileH, const Viewport& viewport, const PerspectiveCamera& camera);
	void setupShader(const GLShader& shader, uint gridTextureIdx, uint lightIdTextureIdx);
	void update(const PerspectiveCamera& camera, const glm::vec4* viewspaceLightPositionRangeList, uint numLights);

	uint getGridWidth() { return m_gridWidth; };
	uint getGridHeight() { return m_gridHeight; };
	uint getGridDepth() { return m_gridDepth; };

private:

	GLint m_recLogSD1Loc;
	GLint m_recNearLoc;

	float m_recLogSD1;
	float m_recNear;

	uint m_gridWidth;
	uint m_gridHeight;
	uint m_gridDepth;
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
	rde::vector<ushort> m_lightIndices;

	GLTextureBuffer m_lightIndiceBuffer;
	GLTextureBuffer m_lightGridBuffer;
};