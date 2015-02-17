#pragma once

class GLMesh;

struct ModelComponent
{
	ModelComponent() {}
	ModelComponent(GLMesh* mesh) : mesh(mesh) {}

	GLMesh* mesh = NULL;
};