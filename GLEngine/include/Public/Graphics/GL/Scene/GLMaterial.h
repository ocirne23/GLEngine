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
	glm::vec4 metalnessMapping;
	glm::vec4 roughnessMapping;
	glm::vec4 opacityMapping;

	int diffuseAtlasIdx;
	int normalAtlasIdx;
	int metalnessAtlasIdx;
	int roughnessAtlasIdx;
	int opacityAtlasIdx;

	int padding0, padding1, padding2;
};