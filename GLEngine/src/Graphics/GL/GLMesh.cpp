#include "Graphics/GL/GLMesh.h"

#include "GLEngine.h"

#include "Graphics/Graphics.h"
#include "Graphics/Pixmap.h"
#include "Graphics/GL/GL.h"

#include "Utils/EAssetType.h"
#include "Utils/FileHandle.h"

#include "Input/Input.h"
#include "Input/EKey.h"

#include <glm/glm.hpp>

BEGIN_UNNAMED_NAMESPACE()

static const glm::vec4 NO_TEX_COLOR(0.8f, 0.8f, 0.8f, 1.0f);

struct Vertex
{
	glm::vec3 position;
	glm::vec2 texcoords;
	glm::vec3 normal;
	glm::vec3 tangents;
	glm::vec3 bitangents;
	uint materialID;
};

byte floatToByteCol(float f)
{
	return (byte) floor(f >= 1.0 ? 255 : f * 255.0);
}

END_UNNAMED_NAMESPACE()

void GLMesh::loadFromDB(const char* a_filePath)
{
	eastl::vector<uint> indices;
	eastl::vector<Vertex> vertices;
	// TODO: Load stuff
	assert(false);

	m_numIndices = indices.size();

	m_stateBuffer.initialize();
	m_stateBuffer.begin();

	m_indiceBuffer.initialize(GLVertexBuffer::EBufferType::ELEMENT_ARRAY, GLVertexBuffer::EDrawUsage::STATIC);
	m_indiceBuffer.upload(sizeof(indices[0]) * indices.size(), &indices[0]);

	m_vertexBuffer.initialize(GLVertexBuffer::EBufferType::ARRAY, GLVertexBuffer::EDrawUsage::STATIC);
	m_vertexBuffer.upload(sizeof(vertices[0]) * vertices.size(), &vertices[0]);

	VertexAttribute attributes[] =
	{
		VertexAttribute(0, "Position", VertexAttribute::EFormat::FLOAT, 3),
		VertexAttribute(1, "Texcoords", VertexAttribute::EFormat::FLOAT, 2),
		VertexAttribute(2, "Normals", VertexAttribute::EFormat::FLOAT, 3),
		VertexAttribute(3, "Tangents", VertexAttribute::EFormat::FLOAT, 3),
		VertexAttribute(4, "Bitangents", VertexAttribute::EFormat::FLOAT, 3),
		VertexAttribute(5, "MaterialID", VertexAttribute::EFormat::UNSIGNED_INT, 1)
	};

	m_vertexBuffer.setVertexAttributes(ARRAY_SIZE(attributes), attributes);
	m_stateBuffer.end();
}

void GLMesh::render()
{
	m_stateBuffer.begin();
	glDrawElements(GL_TRIANGLES, m_numIndices, GL_UNSIGNED_INT, NULL);
	m_stateBuffer.end();
}

// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); wireframe rendering

