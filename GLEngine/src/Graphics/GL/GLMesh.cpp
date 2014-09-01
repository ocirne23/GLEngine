#include "Graphics\GL\GLMesh.h"

#include "GLEngine.h"
#include "Graphics\GL\GL.h"
#include "Graphics\Graphics.h"

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

struct IOMaterialProperty
{
	IOMaterialProperty()
	{
		memset(this, 0, sizeof(IOMaterialProperty));
	};
	glm::vec4 diffuseColorAndAlpha;
	glm::vec4 specularColorAndExp;
	glm::vec4 materialEmissiveAndPower;
	char diffuseTex[_MAX_PATH];
	char bumpTex[_MAX_PATH];
	char specularTex[_MAX_PATH];
	char maskTex[_MAX_PATH];
};
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
	if (m_initialized)
	{
		delete m_vertexBuffer;
		delete m_indiceBuffer;
		delete m_stateBuffer;
		delete m_matUniformBuffer;
	}
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
	rde::vector<IOMaterialProperty> matProperties;
	rde::vector<uint> baseIndices;

	int type;
	file.readBytes(reinterpret_cast<char*>(&type), sizeof(uint), 0);
	assert(type == ResourceType_MODEL);

	file.readBytes(reinterpret_cast<char*>(&m_numOpagueMeshes), sizeof(uint), sizeof(uint));
	uint offset = sizeof(uint) * 2;

	offset = readVector(file, m_indiceCounts, offset);
	offset = readVector(file, baseIndices, offset);
	offset = readVector(file, matProperties, offset);
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

	m_stateBuffer = new GLStateBuffer();
	m_stateBuffer->initialize();
	m_stateBuffer->begin();

	m_indiceBuffer = new GLVertexBuffer();
	m_indiceBuffer->initialize(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW);
	m_indiceBuffer->upload(sizeof(indices[0]) * indices.size(), &indices[0]);

	m_vertexBuffer = new GLVertexBuffer();
	m_vertexBuffer->initialize(GL_ARRAY_BUFFER, GL_STATIC_DRAW);
	m_vertexBuffer->upload(sizeof(vertices[0]) * vertices.size(), &vertices[0]);

	VertexAttribute attributes[] =
	{
		VertexAttribute(0, "Position", VertexAttribute::Format::FLOAT, 3),
		VertexAttribute(1, "Texcoords", VertexAttribute::Format::FLOAT, 2),
		VertexAttribute(2, "Normals", VertexAttribute::Format::FLOAT, 3),
		VertexAttribute(3, "Tangents", VertexAttribute::Format::FLOAT, 3),
		VertexAttribute(4, "Bitangents", VertexAttribute::Format::FLOAT, 3),
		VertexAttribute(5, "MaterialID", VertexAttribute::Format::UNSIGNED_INT, 1)
	};

	m_vertexBuffer->setVertexAttributes(6, attributes);
	m_stateBuffer->end();

	GLTextureManager& textureManager = GLEngine::graphics->getTextureManager();

	rde::string texturePathStr(filePath);
	rde::string::size_type idx = texturePathStr.find_index_of_last('/');
	if (idx == rde::string::npos)
		idx = texturePathStr.find_index_of_last('\\');

	texturePathStr = texturePathStr.substr(0, idx).append("\\");

	m_matProperties.resize(matProperties.size());

	for (int i = 0; i < matProperties.size(); ++i)
	{
		m_matProperties[i].diffuseColorAndAlpha = matProperties[i].diffuseColorAndAlpha;
		m_matProperties[i].specularColorAndExp = matProperties[i].specularColorAndExp;
		m_matProperties[i].materialEmissiveAndPower = matProperties[i].materialEmissiveAndPower;

		if (matProperties[i].diffuseTex[0])
			m_matProperties[i].diffuseHandle = textureManager.getTextureHandle(rde::string(texturePathStr).append(matProperties[i].diffuseTex));
		if (matProperties[i].specularTex[0])
			m_matProperties[i].specularHandle = textureManager.getTextureHandle(rde::string(texturePathStr).append(matProperties[i].specularTex));
		if (matProperties[i].bumpTex[0])
			m_matProperties[i].bumpHandle = textureManager.getTextureHandle(rde::string(texturePathStr).append(matProperties[i].bumpTex));
		if (matProperties[i].maskTex[0])
			m_matProperties[i].maskHandle = textureManager.getTextureHandle(rde::string(texturePathStr).append(matProperties[i].maskTex));
	}

	m_stateBuffer->begin();

	m_matUniformBuffer = new GLConstantBuffer();
	m_matUniformBuffer->initialize(shader, matUBOBindingPoint, "MaterialProperties", GL_STREAM_DRAW);
	m_matUniformBuffer->upload(m_matProperties.size() * sizeof(m_matProperties[0]), &m_matProperties[0]);
	m_matUBOBindingPoint = matUBOBindingPoint;

	m_stateBuffer->end();

	m_initialized = true;
}

void GLMesh::render(bool renderOpague, bool renderTransparent, bool bindMaterials)
{
	m_stateBuffer->begin();
	{
		glDrawElements(GL_TRIANGLES, m_numIndices, GL_UNSIGNED_INT, NULL);
	}
	m_stateBuffer->end();
}