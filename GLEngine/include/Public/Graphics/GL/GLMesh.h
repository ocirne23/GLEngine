#pragma once

#include "Core.h"
#include "Graphics/GL/Wrappers/GLStateBuffer.h"
#include "Graphics/GL/Wrappers/GLTextureArray.h"

#include "3rdparty/rde/rde_string.h"
#include "3rdparty/rde/vector.h"

#include <glm/glm.hpp>

struct MaterialProperty
{
	MaterialProperty() {}

	glm::vec4 diffuseTexMapping;
	glm::vec4 normalTexMapping;
	int diffuseAtlasNr = -1;
	int bumpAtlasNr    = -1;
	int padding        = 0;
	int padding2       = 0;
};

class GLShader;
class GLConstantBuffer;
class GLVertexBuffer;

class GLMesh
{
public:
	GLMesh() {}
	GLMesh(const GLMesh& copy) = delete;
	~GLMesh();

	void loadFromFile(const char* filePath, uint textureUnit, uint matUBOBindingPoint);
	void render(const GLShader& shader, bool renderOpague = true, bool renderTransparent = true, bool bindMaterials = true);

private:

	void initializeUBO(const GLShader& shader);

	bool m_initialized                   = false;
	GLStateBuffer m_stateBuffer;
	GLVertexBuffer* m_indiceBuffer       = NULL;
	GLVertexBuffer* m_vertexBuffer       = NULL;
	uint m_matUBOBindingPoint            = 0;
	GLConstantBuffer* m_matUniformBuffer = NULL;
	uint m_textureUnit                   = 0;
	GLTextureArray m_textureArray;
	uint m_numIndices                    = 0;

	rde::vector<MaterialProperty> m_matProperties;
};