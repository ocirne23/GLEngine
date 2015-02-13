#pragma once

class GLMesh;

struct ModelComponent
{
	ModelComponent() : mesh(0) {}
	ModelComponent(GLMesh* mesh) : mesh(mesh) {}

	GLMesh* mesh;
};