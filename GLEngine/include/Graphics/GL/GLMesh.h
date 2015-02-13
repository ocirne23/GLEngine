#pragma once

#include "Core.h"
#include "Graphics/GL/GLTypes.h"
#include "Graphics/GL/Core/GLStateBuffer.h"
#include "Graphics/GL/Core/GLTextureArray.h"

#include "rde/rde_string.h"
#include "rde/vector.h"

#include <glm/glm.hpp>

struct MaterialProperty
{
	MaterialProperty() {}

	glm::vec4 diffuseTexMapping;
	glm::vec4 bumpTexMapping;
	int diffuseAtlasNr	= -1;
	int bumpAtlasNr		= -1;
	int padding			= 0;
	int padding2		= 0;
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

	void loadFromFile(const char* filePath, uint textureUnit, GLuint matUBOBindingPoint);
	void render(const GLShader& shader, bool renderOpague = true, bool renderTransparent = true, bool bindMaterials = true);

private:

	void uploadMaterialUBO(const GLShader& shader);

	bool m_initialized	= false;

	GLStateBuffer m_stateBuffer;
	GLVertexBuffer* m_indiceBuffer			= NULL;
	GLVertexBuffer* m_vertexBuffer			= NULL;

	uint m_matUBOBindingPoint				= 0;
	GLConstantBuffer* m_matUniformBuffer	= NULL;

	uint m_textureUnit = 0;
	GLTextureArray m_textureArray;

	uint m_numTransparentMeshes = 0;
	uint m_numIndices			= 0;

	rde::vector<MaterialProperty> m_matProperties;
};