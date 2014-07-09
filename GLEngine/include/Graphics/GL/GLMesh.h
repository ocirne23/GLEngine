#pragma once

#include "Core.h"
#include "Graphics\GL\GLTextureManager.h"
#include "Graphics\GL\GLTypes.h"
#include "rde\rde_string.h"
#include "rde\vector.h"

#include <glm\glm.hpp>

class GLShader;
class GLStateBuffer;
class GLConstantBuffer;
class GLVertexBuffer;

struct MaterialProperty
{
	MaterialProperty() : diffuseColorAndAlpha(0), specularColorAndExp(0), materialEmissiveAndPower(0) {};
	glm::vec4 diffuseColorAndAlpha;
	glm::vec4 specularColorAndExp;
	glm::vec4 materialEmissiveAndPower;
	GLTextureManager::TextureHandle diffuseHandle;
	GLTextureManager::TextureHandle bumpHandle;
	GLTextureManager::TextureHandle specularHandle;
	GLTextureManager::TextureHandle maskHandle;
};

class GLMesh
{
public:
	GLMesh();
	GLMesh(const GLMesh& copy) = delete;
	~GLMesh();

	void loadFromFile(const char* filePath, bool flipUV, GLShader& shader, GLuint matUBOBindingPoint = 0, GLuint textureBindOffset = 0);
	void reloadShader(GLShader& shader);
	void render(bool renderOpague = true, bool renderTransparent = true, bool bindMaterials = true);

private:

	void initUniforms(GLShader& shader, GLuint matUBOBindingPoint, GLuint textureBindOffset);

	GLStateBuffer* m_stateBuffer;
	GLVertexBuffer* m_indiceBuffer;
	GLVertexBuffer* m_vertexBuffer;
	GLConstantBuffer* m_matUniformBuffer;

	GLTextureManager::TextureBinder* m_textureBinder;
	unsigned int m_textureBindOffset;
	unsigned int m_matUBOBindingPoint;
	GLint m_textureDataLoc;

	unsigned int m_numOpagueMeshes;
	rde::vector<GLsizei> m_indiceCounts;
	rde::vector<GLvoid*> m_baseIndices;
	rde::vector<GLint> m_baseVertices;
	rde::vector<MaterialProperty> m_matProperties;
};