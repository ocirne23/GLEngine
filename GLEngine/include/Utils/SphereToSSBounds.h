#pragma once

#include <glm/glm.hpp>

#include "Utils/Viewport.h"
#include "Utils/Bounds2D.h"
#include "Utils/Bounds3D.h"

class Camera;
class PerspectiveCamera;

IBounds2D findScreenSpaceBounds(const Camera& camera, glm::vec3 lightPosWorldSpace, float lightRadius, Viewport viewport);
IBounds3D findScreenSpace3DTile(const PerspectiveCamera& camera, glm::vec3 lightPosViewSpace, float lightRadius, Viewport viewport, glm::ivec2 pixelsPerTile, float recLogSD1);