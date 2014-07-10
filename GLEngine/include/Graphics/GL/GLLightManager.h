#pragma once

#include "rde\vector.h"
#include "Graphics\GL\GLTypes.h"
#include "Graphics\GL\Core\GLConstantBuffer.h"
#include "Graphics\GL\Core\GLTextureBuffer.h"
#include "Utils\Viewport.h"

#include <glm\glm.hpp>

typedef unsigned int LightHandle;

class GLShader;
class PerspectiveCamera;

class GLLightManager
{
public:
	GLLightManager() : m_numUsedLights(0), m_gridWidth(0), m_gridHeight(0), m_gridDepth(0),
		m_maxLights(0), m_numLightsLoc(0), m_camera(0)
	{};
	~GLLightManager() {};

	void initialize(unsigned int m_maxLights, unsigned int pixelsPerTileW, unsigned int pixelsPerTileH, const PerspectiveCamera& camera, const Viewport& viewport);
	void setupShader(GLShader& shader);

	void update(const PerspectiveCamera& camera, float deltaSec);

	unsigned int getGridWidth() { return m_gridWidth; };
	unsigned int getGridHeight() { return m_gridHeight; };
	unsigned int getGridDepth() { return m_gridDepth; };


	unsigned int getNumLights() { return m_numUsedLights; };

	LightHandle createLight(glm::vec3 pos, glm::vec3 color, float invRadius);
	void deleteLight(LightHandle light);
	void deleteLights();

	void setLightPosition(LightHandle light, const glm::vec3& position);
	void setLightRange(LightHandle light, float range);
	void setLightColor(LightHandle light, const glm::vec3& color);
	glm::vec3 getLightPosition(LightHandle light);
	float getLightRange(LightHandle light);
	glm::vec3 getLightColor(LightHandle light);

private:

	void updateTiles(const PerspectiveCamera& camera, float deltaSec);

	static const unsigned int AVAILABLE_HANDLE = 0xFFFFFFFF;

	unsigned int m_maxLights;
	unsigned int m_numUsedLights;

	rde::vector<glm::vec4> m_lightPositionRanges;
	rde::vector<glm::vec4> m_viewspaceLightPositionRanges;
	rde::vector<glm::vec4> m_lightColors;

	rde::vector<unsigned short> m_usedLightRefs;
	rde::vector<unsigned short> m_lightHandleIndices;

	GLint m_numLightsLoc;
	GLint m_recLogSD1Loc;
	GLint m_recNearLoc;

	struct LightListRef
	{
		unsigned int begin;
		unsigned int end;
	};
	rde::vector<LightListRef> m_lightGrid;
	rde::vector<rde::vector<unsigned short>> m_tileLightIndices;
	rde::vector<unsigned short> m_lightIndices;

	Viewport m_viewport;
	const PerspectiveCamera* m_camera;

	float m_recLogSD1;

	unsigned int m_gridWidth;
	unsigned int m_gridHeight;
	unsigned int m_gridDepth;
	unsigned int m_gridSize;
	glm::ivec2 m_pixelsPerTile;

	GLTextureBuffer m_lightIndiceBuffer;
	GLTextureBuffer m_lightGridBuffer;

	GLConstantBuffer m_lightPositionRangeBuffer;
	GLConstantBuffer m_lightColorBuffer;
};