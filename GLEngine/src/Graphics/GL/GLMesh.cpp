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

#include "rde\rde_string.h"

#include <glm\glm.hpp>

enum { MAX_MATERIALS = 200 };

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

GLMesh::GLMesh() : m_numOpagueMeshes(0)
{
}

GLMesh::~GLMesh()
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

void GLMesh::loadFromFile(const char* filePath, GLShader& shader, GLuint matUBOBindingPoint, GLuint textureBindOffset)
{
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
	
	file.readBytes(reinterpret_cast<char*>(&m_numOpagueMeshes), sizeof(uint));
	readVector(file, m_indiceCounts);
	readVector(file, baseIndices);
	readVector(file, m_baseVertices);
	readVector(file, matProperties);
	readVector(file, indices);
	readVector(file, vertices);

	uint numBaseIndices = baseIndices.size();
	m_baseIndices.resize(numBaseIndices);
	for (uint i = 0; i < numBaseIndices; ++i)
	{
		m_baseIndices[i] = (GLvoid*) baseIndices[i];
	}

	file.close();

	print("numvertices: %i numindices: %i \n", vertices.size(), indices.size());
	print("%f %f %f %i %i %i\n", vertices[0].position.x, vertices[0].position.y, vertices[0].position.z, indices[0], indices[1], indices[2]);

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
	
	CHECK_GL_ERROR();
	GLTextureManager& textureManager = GLEngine::graphics->getTextureManager();
	m_textureBinder = textureManager.createTextureBinder();

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
			m_matProperties[i].diffuseHandle = m_textureBinder->createTextureHandle(rde::string(texturePathStr).append(matProperties[i].diffuseTex).c_str());
		if (matProperties[i].specularTex[0])
			m_matProperties[i].specularHandle = m_textureBinder->createTextureHandle(rde::string(texturePathStr).append(matProperties[i].specularTex).c_str());
		if (matProperties[i].bumpTex[0])
			m_matProperties[i].bumpHandle = m_textureBinder->createTextureHandle(rde::string(texturePathStr).append(matProperties[i].bumpTex).c_str());
		if (matProperties[i].maskTex[0])
			m_matProperties[i].maskHandle = m_textureBinder->createTextureHandle(rde::string(texturePathStr).append(matProperties[i].maskTex).c_str());
	}
	
	initUniforms(shader, matUBOBindingPoint, textureBindOffset);
}
void GLMesh::initUniforms(GLShader& shader, GLuint matUBOBindingPoint, GLuint textureBindOffset)
{
	m_stateBuffer->begin();

	m_matUniformBuffer = new GLConstantBuffer();
	m_matUniformBuffer->initialize(shader, matUBOBindingPoint, "MaterialProperties", GL_STREAM_DRAW);
	m_matUniformBuffer->upload(m_matProperties.size() * sizeof(m_matProperties[0]), &m_matProperties[0]);
	m_matUBOBindingPoint = matUBOBindingPoint;
	m_textureBindOffset = textureBindOffset;

	m_textureDataLoc = glGetUniformLocation(shader.getID(), "u_textureData");

	m_stateBuffer->end();
}

void GLMesh::reloadShader(GLShader& shader)
{
	assert(shader.isBegun());
	initUniforms(shader, m_matUBOBindingPoint, m_textureBindOffset);
}

void GLMesh::render(bool renderOpague, bool renderTransparent, bool bindMaterials)
{
	m_stateBuffer->begin();
	{
		if (bindMaterials)
		{
			m_textureBinder->bindTextureArrays(m_textureDataLoc, m_textureBindOffset, GLEngine::graphics->getMaxTextureUnits());
		}

		CHECK_GL_ERROR();
		if (renderOpague)
			glMultiDrawElementsBaseVertex(GL_TRIANGLES, &m_indiceCounts[0], GL_UNSIGNED_INT, &m_baseIndices[0], m_baseIndices.size() - m_numOpagueMeshes, &m_baseVertices[0]);
		CHECK_GL_ERROR();
		if (renderTransparent)
			glMultiDrawElementsBaseVertex(GL_TRIANGLES, &m_indiceCounts.back() - m_numOpagueMeshes, GL_UNSIGNED_INT, &m_baseIndices.back() - m_numOpagueMeshes, m_numOpagueMeshes, &m_baseVertices.back() - m_numOpagueMeshes);
	}
	m_stateBuffer->end();
}