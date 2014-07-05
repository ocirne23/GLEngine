#pragma once

template <typename T>
struct TBounds2D
{
	T minX, minY, maxX, maxY;
};

typedef TBounds2D<int> IBounds2D;
typedef TBounds2D<float> Bounds2D;