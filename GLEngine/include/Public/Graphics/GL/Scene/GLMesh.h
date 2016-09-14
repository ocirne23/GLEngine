#pragma once

#include "Core.h"
#include "Graphics/GL/Wrappers/GLStateBuffer.h"
#include "Graphics/GL/Wrappers/GLVertexBuffer.h"

#include <glm/glm.hpp>

class DBMesh;
class GLConstantBuffer;
class GLVertexBuffer;

class GLMesh
{
public:
	GLMesh() {}
	GLMesh(const GLMesh& copy);
	~GLMesh() {};

	void initialize(const DBMesh& mesh);
	void render();

	const glm::vec3& getBoundsMin() const { return m_boundsMin; }
	const glm::vec3& getBoundsMax() const { return m_boundsMax; }
private:

	GLStateBuffer m_stateBuffer;
	GLVertexBuffer m_indiceBuffer;
	GLVertexBuffer m_vertexBuffer;
	uint m_numIndices = 0;
	glm::vec3 m_boundsMin;
	glm::vec3 m_boundsMax;
};