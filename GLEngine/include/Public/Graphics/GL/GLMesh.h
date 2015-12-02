#pragma once

#include "Core.h"
#include "Graphics/GL/Wrappers/GLConstantBuffer.h"
#include "Graphics/GL/Wrappers/GLShader.h"
#include "Graphics/GL/Wrappers/GLStateBuffer.h"
#include "Graphics/GL/Wrappers/GLTextureArray.h"
#include "Graphics/GL/Wrappers/GLVertexBuffer.h"
#include "EASTL/vector.h"

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

	void loadFromDB(const char* filePath);
	void render();

private:

	void initializeUBO(const GLShader& shader);

private:

	GLStateBuffer m_stateBuffer;
	GLVertexBuffer m_indiceBuffer;
	GLVertexBuffer m_vertexBuffer;
	uint m_numIndices = 0;
};