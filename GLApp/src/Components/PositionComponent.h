#pragma once

#include <glm/glm.hpp>

struct PositionComponent
{
	PositionComponent() : pos(1.0f) {}
	PositionComponent(float x, float y, float z) : pos(x, y, z) {}

	glm::vec3 pos;
};