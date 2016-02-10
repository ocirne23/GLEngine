#pragma once

#include "Core.h"
#include "EASTL/vector.h"
#include "Graphics/GL/Wrappers/GLConstantBuffer.h"
#include "Graphics/GL/Wrappers/GLTextureBuffer.h"

class PerspectiveCamera;
class LightManager;

class ClusteredShading
{
public:

	struct GlobalsUBO
	{
		float u_recNear;
		float u_recLogSD1;

		int u_tileWidth;
		int u_tileHeight;

		int u_gridHeight;
		int u_gridDepth;
	};

public:
	ClusteredShading() {}
	ClusteredShading(const ClusteredShading& copy) = delete;
	~ClusteredShading();

	void initialize(const PerspectiveCamera& camera, uint screenWidth, uint screenHeight);
	void update(const PerspectiveCamera& camera, const LightManager& lightManager);
	void bindTextureBuffers();

	uint getTileWidth() const  { return m_pixelsPerTileW; }
	uint getTileHeight() const { return m_pixelsPerTileH; }
	uint getGridWidth() const  { return m_gridWidth; }
	uint getGridHeight() const { return m_gridHeight; }
	uint getGridDepth() const  { return m_gridDepth; }
	uint getGridSize() const   { return m_gridSize; }

private:

	bool m_initialized        = false;
	uint m_screenWidth        = 0;
	uint m_screenHeight       = 0;
	uint m_gridWidth          = 0;
	uint m_gridHeight         = 0;
	uint m_gridDepth          = 0;
	uint m_gridSize           = 0;
	uint m_pixelsPerTileW     = 0;
	uint m_pixelsPerTileH     = 0;
	uint m_maxNumLightIndices = 0;
	float m_recLogSD1         = 0.0f;
	float m_recNear           = 0.0f;

	enum { MAX_NUM_INDICES_PER_TILE = 10 };

	struct TileIndiceList
	{
		ushort count;
		ushort indices[MAX_NUM_INDICES_PER_TILE];
		inline void push_back(ushort val)
		{
			if (count < MAX_NUM_INDICES_PER_TILE)
				indices[count++] = val;
		}
	};
	eastl::vector<TileIndiceList> m_tileLightIndices;

	GLConstantBuffer m_lightPositionRangesUBO;
	GLConstantBuffer m_lightColorIntensitiesUBO;
	GLConstantBuffer m_clusteredShadingGlobalsUBO;

	GLTextureBuffer m_lightIndiceTextureBuffer;
	GLTextureBuffer m_lightGridTextureBuffer;
};