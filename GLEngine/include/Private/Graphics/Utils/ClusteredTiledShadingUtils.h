#pragma once

#include "Core.h"

#include <glm/glm.hpp>

class Camera;
class PerspectiveCamera;

template <typename T>
struct TBounds2D
{
	T minX = (T) 0;
	T minY = (T) 0;
	T maxX = (T) 0;
	T maxY = (T) 0;
};

template <typename T>
struct TBounds3D
{
	T minX = (T) 0;
	T minY = (T) 0;
	T minZ = (T) 0;
	T maxX = (T) 0;
	T maxY = (T) 0;
	T maxZ = (T) 0;
};

typedef TBounds2D<int> IBounds2D;
typedef TBounds3D<int> IBounds3D;

class ClusteredTiledShadingUtils
{
public:

	static IBounds2D sphereToScreenSpaceBounds2D(const PerspectiveCamera& camera, const glm::vec3& lightPosWorldSpace, float lightRadius, uint screenWidth, uint screenHeight);
	static IBounds3D sphereToScreenSpaceBounds3D(const PerspectiveCamera& camera, const glm::vec3& lightPosViewSpace, float lightRadius, uint screenWidth, uint screenHeight, 
												 uint pixelsPerTileW, uint pixelsPerTileH, float recLogSD1);
private:
	ClusteredTiledShadingUtils() {}
};