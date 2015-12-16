#pragma once

#include "Core.h"

#include <glm/glm.hpp>

class DBMaterial;

struct GLMaterial
{
	GLMaterial() {}
	void initialize(const DBMaterial& material);

	glm::vec4 diffuseMapping;
	glm::vec4 normalMapping;
	int diffuseAtlasIdx;
	int normalAtlasIdx;
	float smoothness;
	float metalness;
	glm::vec4 materialColor;
};