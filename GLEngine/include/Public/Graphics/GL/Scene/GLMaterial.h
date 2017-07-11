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

	int diffuseAtlasIdx   = -1;
	int normalAtlasIdx    = -1;
	int metalnessAtlasIdx = -1;
	int roughnessAtlasIdx = -1;
	int opacityAtlasIdx   = -1;

	int padding0, padding1, padding2;
};