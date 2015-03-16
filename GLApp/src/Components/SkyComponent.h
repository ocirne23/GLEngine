#pragma once

class GLMesh;

struct SkyComponent
{
	SkyComponent() {}
	SkyComponent(GLMesh* mesh) : mesh(mesh) {}

	bool centerOnCamera = true;
	GLMesh* mesh        = NULL;
};