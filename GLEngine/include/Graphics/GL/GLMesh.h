#pragma once

#include "Core.h"
#include "Graphics\GL\GLTypes.h"
#include "rde\rde_string.h"
#include "rde\vector.h"
#include "Graphics\GL\Core\GLTextureArray.h"

#include <glm\glm.hpp>

class GLShader;
class GLStateBuffer;
class GLConstantBuffer;
class GLVertexBuffer;

struct MaterialProperty
{
	MaterialProperty() : diffuseAtlasNr(-1), bumpAtlasNr(-1), specularAtlasNr(-1), maskAtlasNr(-1) {};
	glm::vec4 diffuseTexMapping;
	glm::vec4 bumpTexMapping;
	glm::vec4 specTexMapping;
	glm::vec4 maskTexMapping;
	int diffuseAtlasNr;
	int bumpAtlasNr;
	int specularAtlasNr;
	int maskAtlasNr;
};

class GLMesh
{
public:
	GLMesh();
	GLMesh(const GLMesh& copy) = delete;
	~GLMesh();

	void loadFromFile(const char* filePath, GLShader& shader, GLuint matUBOBindingPoint = 0, GLuint textureBindOffset = 0);
	void reloadShader(GLShader& shader);
	void render(bool renderOpague = true, bool renderTransparent = true, bool bindMaterials = true);

private:

	bool m_initialized;
	GLStateBuffer m_stateBuffer;
	GLVertexBuffer m_indiceBuffer;
	GLVertexBuffer m_vertexBuffer;
	GLConstantBuffer m_matUniformBuffer;

	rde::vector<GLTextureArray> m_textureArrays;

	unsigned int m_matUBOBindingPoint;

	unsigned int m_numOpagueMeshes;
	unsigned int m_numIndices;
	rde::vector<GLsizei> m_indiceCounts;
	rde::vector<GLvoid*> m_baseIndices;
	rde::vector<GLint> m_baseVertices;
	rde::vector<MaterialProperty> m_matProperties;
};