#pragma once

#include "Core.h"

class GLMesh;

class Scene
{
public:
	Scene();
	~Scene();
	Scene(const Scene& copy) = delete;

public:

	void addMesh(const GLMesh& mesh);
	void removeMesh(const GLMesh& mesh);

private:
};