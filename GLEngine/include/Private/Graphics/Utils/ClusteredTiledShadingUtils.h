#pragma once

#include "Core.h"

#include <glm/glm.hpp>

class Camera;
class PerspectiveCamera;

struct IBounds2D
{
	IBounds2D() {}
	IBounds2D(const glm::ivec2& min, const glm::ivec2& max) : min(min), max(max) {}

	glm::ivec2 min;
	glm::ivec2 max;

	void clamp(const glm::ivec2& a_min, const glm::ivec2& a_max)
	{
		min = glm::clamp(min, a_min, a_max);
		max = glm::clamp(max, a_min, a_max);
	}
};

struct IBounds3D
{
	IBounds3D() {}
	IBounds3D(const glm::ivec3& min, const glm::ivec3& max) : min(min), max(max) {}

	glm::ivec3 min;
	glm::ivec3 max;

	void clamp(const glm::ivec3& a_min, const glm::ivec3& a_max)
	{
		min = glm::clamp(min, a_min, a_max);
		max = glm::clamp(max, a_min, a_max);
	}
};

class ClusteredTiledShadingUtils
{
public:

	static IBounds2D sphereToScreenSpaceBounds2D(const PerspectiveCamera& camera, const glm::vec3& lightPosWorldSpace, float lightRadius, uint screenWidth, uint screenHeight);
	static IBounds3D sphereToScreenSpaceBounds3D(const PerspectiveCamera& camera, const glm::vec3& lightPosViewSpace, float lightRadius, uint screenWidth, uint screenHeight, 
												 uint pixelsPerTileW, uint pixelsPerTileH, float recLogSD1);
private:
	ClusteredTiledShadingUtils() {}
};