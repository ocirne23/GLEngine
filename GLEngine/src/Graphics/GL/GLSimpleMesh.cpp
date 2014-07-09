#include "Graphics\GL\GLSimpleMesh.h"

#include "GLEngine.h"
#include "Graphics\GL\GL.h"
#include "Graphics\Graphics.h"

#include "Graphics\GL\Core\GLShader.h"
#include "Graphics\GL\Core\GLVertexBuffer.h"
#include "Graphics\GL\Core\GLConstantBuffer.h"
#include "Graphics\GL\Core\GLStateBuffer.h"

#include "Utils\FileHandle.h"
#include "Utils\CheckGLError.h"

#include "rde\rde_string.h"

#include <glm\glm.hpp>

GLSimpleMesh::GLSimpleMesh()
{
}

GLSimpleMesh::~GLSimpleMesh()
{
}

template <typename T>
void readVector(FileHandle& handle, rde::vector<T>& vector)
{
	int size;
	handle.readBytes(reinterpret_cast<char*>(&size), sizeof(int));
	print("readVector %i \n", size);
	vector.resize(size);
	handle.readBytes(reinterpret_cast<char*>(&vector[0]), size * sizeof(vector[0]));
}

void GLSimpleMesh::loadFromFile(const char* filePath)
{
	FileHandle file(filePath);
	if (!file.exists())
	{
		print("Mesh file does not exist: %s \n", filePath);
		return;
	}

	rde::vector<uint> indices;
	rde::vector<glm::vec3> vertices;
	rde::vector<uint> baseIndices;

	readVector(file, m_indiceCounts);
	readVector(file, baseIndices);
	readVector(file, m_baseVertices);
	readVector(file, indices);
	readVector(file, vertices);
	file.close();

	uint numBaseIndices = baseIndices.size();
	m_baseIndices.resize(numBaseIndices);
	for (uint i = 0; i < numBaseIndices; ++i)
	{
		m_baseIndices[i] = (GLvoid*) baseIndices[i];
	}

	m_numIndices = indices.size();

	print("numvertices: %i numindices: %i \n", vertices.size(), indices.size());
	print("%f %f %f %i %i %i\n", vertices[0].x, vertices[0].y, vertices[0].z, indices[0], indices[1], indices[2]);

	CHECK_GL_ERROR();

	m_stateBuffer = new GLStateBuffer();
	m_stateBuffer->initialize();
	m_stateBuffer->begin();
	CHECK_GL_ERROR();

	m_indiceBuffer = new GLVertexBuffer();
	m_indiceBuffer->initialize(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW);
	m_indiceBuffer->upload(sizeof(indices[0]) * indices.size(), &indices[0]);
	CHECK_GL_ERROR();

	m_vertexBuffer = new GLVertexBuffer();
	m_vertexBuffer->initialize(GL_ARRAY_BUFFER, GL_STATIC_DRAW);
	m_vertexBuffer->upload(sizeof(vertices[0]) * vertices.size(), &vertices[0]);
	CHECK_GL_ERROR();

	VertexAttribute attributes [] =
	{
		VertexAttribute(0, "Position", VertexAttribute::Format::FLOAT, 3),
	};

	m_vertexBuffer->setVertexAttributes(1, attributes);
	m_stateBuffer->end();
}

void GLSimpleMesh::render()
{
	m_stateBuffer->begin();
	{
		m_indiceBuffer->bind();
		m_vertexBuffer->bind();
		
		glMultiDrawElementsBaseVertex(GL_TRIANGLES, &m_indiceCounts[0], GL_UNSIGNED_INT, &m_baseIndices[0], m_baseIndices.size(), &m_baseVertices[0]);
	}
	m_stateBuffer->end();
}