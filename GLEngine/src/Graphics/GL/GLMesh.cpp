#include "Graphics\GL\GLMesh.h"

#include "GLEngine.h"
#include "Graphics\GL\GL.h"
#include "Graphics\Graphics.h"

#include "Graphics\Pixmap.h"
#include "Graphics\GL\Core\GLShader.h"
#include "Graphics\GL\Core\GLTextureArray.h"
#include "Graphics\GL\Core\GLVertexBuffer.h"
#include "Graphics\GL\Core\GLConstantBuffer.h"
#include "Graphics\GL\Core\GLStateBuffer.h"

#include "Utils\FileHandle.h"
#include "Utils\CheckGLError.h"
#include "Utils\ResourceType.h"

#include "rde\rde_string.h"
#include "Input\Input.h"
#include "Input\Key.h"

#include <glm\glm.hpp>

enum { MAX_MATERIALS = 200 };

#ifdef ANDROID
#define _MAX_PATH 260
#endif

struct MeshEntry
{
	unsigned int meshIndex;
	unsigned int numIndices;
	unsigned int baseVertex;
	unsigned int baseIndex;
	unsigned int materialIndex;
};
struct Vertex
{
	glm::vec3 position;
	glm::vec2 texcoords;
	glm::vec3 normal;
	glm::vec3 tangents;
	glm::vec3 bitangents;
	unsigned int materialID;
};

GLMesh::GLMesh() : m_numOpagueMeshes(0), m_initialized(false)
{
}

GLMesh::~GLMesh()
{

}

template <typename T>
uint readVector(const FileHandle& handle, rde::vector<T>& vector, uint offset)
{
	int size;
	handle.readBytes(reinterpret_cast<char*>(&size), sizeof(int), offset);
	vector.resize(size);
	handle.readBytes(reinterpret_cast<char*>(&vector[0]), size * sizeof(vector[0]), offset + sizeof(int));

	return offset + sizeof(int) + size * sizeof(vector[0]);
}

void GLMesh::loadFromFile(const char* filePath, GLShader& shader, GLuint matUBOBindingPoint, GLuint textureBindOffset)
{
	assert(!m_initialized);

	FileHandle file(filePath);
	if (!file.exists())
	{
		print("Mesh file does not exist: %s \n", filePath);
		return;
	}

	rde::vector<uint> indices;
	rde::vector<Vertex> vertices;
	rde::vector<uint> baseIndices;

	int type;
	file.readBytes(reinterpret_cast<char*>(&type), sizeof(int), 0);
	assert(type == ResourceType_MODEL);

	int numAtlasses;
	file.readBytes(reinterpret_cast<char*>(&numAtlasses), sizeof(int), 0);

	file.readBytes(reinterpret_cast<char*>(&m_numOpagueMeshes), sizeof(uint), sizeof(uint));
	uint offset = sizeof(uint) * 2;

	offset = readVector(file, m_indiceCounts, offset);
	offset = readVector(file, baseIndices, offset);
	offset = readVector(file, m_matProperties, offset);
	offset = readVector(file, indices, offset);
	offset = readVector(file, vertices, offset);
	file.close();

	m_numIndices = indices.size();

	uint numBaseIndices = baseIndices.size();
	m_baseIndices.resize(numBaseIndices);
	for (uint i = 0; i < numBaseIndices; ++i)
	{
		m_baseIndices[i] = (GLvoid*) baseIndices[i];
	}

	m_stateBuffer.initialize();
	m_stateBuffer.begin();

	m_indiceBuffer.initialize(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW);
	m_indiceBuffer.upload(sizeof(indices[0]) * indices.size(), &indices[0]);

	m_vertexBuffer.initialize(GL_ARRAY_BUFFER, GL_STATIC_DRAW);
	m_vertexBuffer.upload(sizeof(vertices[0]) * vertices.size(), &vertices[0]);

	VertexAttribute attributes[] =
	{
		VertexAttribute(0, "Position", VertexAttribute::Format::FLOAT, 3),
		VertexAttribute(1, "Texcoords", VertexAttribute::Format::FLOAT, 2),
		VertexAttribute(2, "Normals", VertexAttribute::Format::FLOAT, 3),
		VertexAttribute(3, "Tangents", VertexAttribute::Format::FLOAT, 3),
		VertexAttribute(4, "Bitangents", VertexAttribute::Format::FLOAT, 3),
		VertexAttribute(5, "MaterialID", VertexAttribute::Format::UNSIGNED_INT, 1)
	};

	m_vertexBuffer.setVertexAttributes(6, attributes);
	m_stateBuffer.end();

	GLTextureManager& textureManager = GLEngine::graphics->getTextureManager();

	rde::string atlasBasePath(filePath);
	atlasBasePath = atlasBasePath.substr(0, atlasBasePath.find_index_of_last('.') - 1);

	rde::vector<Pixmap> atlasPixmaps(numAtlasses);
	for (int i = 0; i < numAtlasses; ++i)
	{
		rde::string atlasPath = atlasBasePath.append("-atlas-").append(rde::to_string(i)).append(".da");
		atlasPixmaps[i].read(FileHandle(atlasPath));
		assert(atlasPixmaps[i].exists());
	}

	m_stateBuffer.begin();

	m_matUniformBuffer.initialize(shader, matUBOBindingPoint, "MaterialProperties", GL_STREAM_DRAW);
	m_matUniformBuffer.upload(m_matProperties.size() * sizeof(m_matProperties[0]), &m_matProperties[0]);
	m_matUBOBindingPoint = matUBOBindingPoint;

	m_stateBuffer.end();

	m_initialized = true;
}

void GLMesh::render(bool renderOpague, bool renderTransparent, bool bindMaterials)
{
	m_stateBuffer.begin();
	{
		glDrawElements(GL_TRIANGLES, m_numIndices, GL_UNSIGNED_INT, NULL);
	}
	m_stateBuffer.end();
}