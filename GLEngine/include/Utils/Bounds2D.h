#pragma once

template <typename T>
struct TBounds2D
{
    T minX = (T) 0;
    T minY = (T) 0;
    T maxX = (T) 0;
    T maxY = (T) 0;
};

typedef TBounds2D<int> IBounds2D;
typedef TBounds2D<float> Bounds2D;