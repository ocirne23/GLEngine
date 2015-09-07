#pragma once

#include "Core.h"
#include "Graphics/GL/Wrappers/GLConstantBuffer.h"
#include "Graphics/GL/Wrappers/GLShader.h"
#include "Graphics/GL/Wrappers/GLStateBuffer.h"
#include "Graphics/GL/Wrappers/GLTextureArray.h"
#include "Graphics/GL/Wrappers/GLVertexBuffer.h"
#include "3rdparty/rde/rde_string.h"
#include "3rdparty/rde/vector.h"

#include <glm/glm.hpp>

struct MaterialProperty;
class GLShader;
class GLConstantBuffer;
class GLVertexBuffer;

class GLMesh
{
public:
	GLMesh() {}
	GLMesh(const GLMesh& copy) = delete;
	~GLMesh() {};

	void loadFromFile(const char* filePath, uint textureUnit, uint matUBOBindingPoint);
	void render(const GLShader& shader, bool renderOpague = true, bool renderTransparent = true, bool bindMaterials = true);

private:

	void initializeUBO(const GLShader& shader);

private:

	bool m_initialized = false;

	GLStateBuffer m_stateBuffer;
	GLVertexBuffer m_indiceBuffer;
	GLVertexBuffer m_vertexBuffer;
	GLTextureArray m_textureArray;
	GLConstantBuffer m_matUniformBuffer;

	uint m_matUBOBindingPoint = 0;
	uint m_textureUnit        = 0;
	uint m_numIndices         = 0;

	struct MaterialProperty
	{
		glm::vec4 diffuseTexMapping;
		glm::vec4 normalTexMapping;
		int diffuseAtlasNr = -1;
		int bumpAtlasNr = -1;
		int padding = 0;
		int padding2 = 0;
	};

	rde::vector<MaterialProperty> m_matProperties;
};