#pragma once

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

typedef TBounds3D<int> IBounds3D;
typedef TBounds3D<float> Bounds3D;