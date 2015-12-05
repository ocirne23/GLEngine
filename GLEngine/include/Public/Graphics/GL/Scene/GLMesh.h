#pragma once

#include "Core.h"
#include "Graphics/GL/Wrappers/GLStateBuffer.h"
#include "Graphics/GL/Wrappers/GLVertexBuffer.h"

class DBMesh;
class GLConstantBuffer;
class GLVertexBuffer;

class GLMesh
{
public:
	GLMesh() {}
	GLMesh(const GLMesh& copy);
	~GLMesh() {};

	void initialize(const DBMesh& mesh);
	void render();

private:

	GLStateBuffer m_stateBuffer;
	GLVertexBuffer m_indiceBuffer;
	GLVertexBuffer m_vertexBuffer;
	uint m_numIndices = 0;
};