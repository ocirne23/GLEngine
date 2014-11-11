#pragma once

#include "glm\glm.hpp"

class GLMesh;

class Model
{
public:
	Model();
	~Model();
	Model(const Model& copy) = delete;

private:

	glm::mat4 transform;
	GLMesh* mesh;
};