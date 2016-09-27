#include "Graphics/GL/Scene/GLMesh.h"

#include "Database/Assets/DBMesh.h"
#include "Graphics/GL/GL.h"
#include "Graphics/GL/GLTypes.h"
#include "Graphics/GL/Scene/GLConfig.h"

void GLMesh::initialize(const DBMesh& a_mesh)
{
	const eastl::vector<DBMesh::Vertex>& vertices = a_mesh.getVertices();
	const eastl::vector<uint>& indices = a_mesh.getIndices();
	m_numIndices = uint(indices.size());
	m_boundsMin = a_mesh.getBoundsMin();
	m_boundsMax = a_mesh.getBoundsMax();

	m_stateBuffer.initialize();
	m_stateBuffer.begin();
	
	m_vertexBuffer.initialize(GLConfig::getVBOConfig(GLConfig::EVBOs::GLMeshVertex));
	m_vertexBuffer.upload(as_span(rcast<const byte*>(&vertices[0]), vertices.size_bytes()));

	m_indiceBuffer.initialize(GLConfig::getVBOConfig(GLConfig::EVBOs::GLMeshIndice));
	m_indiceBuffer.upload(as_span(rcast<const byte*>(&indices[0]), indices.size_bytes()));

	m_stateBuffer.end();
}

void GLMesh::render()
{
	m_stateBuffer.begin();
	glDrawElements(GL_TRIANGLES, m_numIndices, GL_UNSIGNED_INT, NULL);
	m_stateBuffer.end();
}

GLMesh::GLMesh(const GLMesh& copy)
{
	assert(!m_vertexBuffer.isInitialized());
	assert(!m_indiceBuffer.isInitialized());
}

// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); wireframe rendering

