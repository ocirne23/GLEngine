#pragma once

template <typename T>
struct TBounds3D
{
	T minX, minY, minZ, maxX, maxY, maxZ;
};

typedef TBounds3D<int> IBounds3D;
typedef TBounds3D<float> Bounds3D;