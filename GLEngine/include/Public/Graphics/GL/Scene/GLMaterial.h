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
	uint diffuseAtlasIdx;
	uint normalAtlasIdx;
	float roughness;
	float metalness;
};