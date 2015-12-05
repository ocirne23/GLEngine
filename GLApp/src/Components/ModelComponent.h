#pragma once

#include "EASTL/string.h"

class GLMesh;

struct ModelComponent
{
	ModelComponent() {}
	ModelComponent(const eastl::string& filePath) : filePath(filePath) {}

	eastl::string filePath;
	GLMesh* mesh = NULL;
};