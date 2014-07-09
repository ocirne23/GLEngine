#pragma once

#include "Core.h"
#include "Graphics\GL\GLTypes.h"
#include "rde\rde_string.h"
#include "rde\vector.h"

#include <glm\glm.hpp>

class GLShader;
class GLStateBuffer;
class GLConstantBuffer;
class GLVertexBuffer;

class GLSimpleMesh
{
public:
	GLSimpleMesh();
	GLSimpleMesh(const GLSimpleMesh& copy) = delete;
	~GLSimpleMesh();

	void loadFromFile(const char* filePath);
	void render();

private:

	GLStateBuffer* m_stateBuffer;
	GLVertexBuffer* m_indiceBuffer;
	GLVertexBuffer* m_vertexBuffer;

	uint m_numIndices;
	rde::vector<GLsizei> m_indiceCounts;
	rde::vector<GLvoid*> m_baseIndices;
	rde::vector<GLint> m_baseVertices;
};