#pragma once

#include <glm/gtc/matrix_transform.hpp>

class GLMesh;

class Model
{
public:
	Model();
	~Model();
	Model(const Model& copy) = delete;

public:
	glm::mat4 transform;
	GLMesh* mesh;
};