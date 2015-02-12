#pragma once

#include "Core.h"
#include "Utils/Bounds2D.h"
#include "Utils/Bounds3D.h"

#include <glm/glm.hpp>

class Camera;
class PerspectiveCamera;

IBounds2D sphereToScreenSpaceBounds2D(const Camera& camera, glm::vec3 lightPosWorldSpace, float lightRadius, uint screenWidth, uint screenHeight);
IBounds3D sphereToScreenSpaceBounds3D(const PerspectiveCamera& camera, glm::vec3 lightPosViewSpace, float lightRadius, uint screenWidth, uint screenHeight, uint pixelsPerTileW, uint pixelsPerTileH, float recLogSD1);