#pragma once

#include "3rdparty/rde/rde_string.h"

class GLMesh;

struct ModelComponent
{
	ModelComponent() {}
	ModelComponent(const rde::string& filePath) : filePath(filePath) {}

	rde::string filePath;
	GLMesh* mesh = NULL;
};