#include "Database/Assets/Mesh.h"

Mesh::Mesh(const std::string& a_name) : m_name(a_name)
{

}

void Mesh::addVerticesIndices(const aiMesh* a_mesh, const aiMatrix3x3* a_vertexTransform)
{
	const uint numVertices = a_mesh->mNumVertices;
	const uint numFaces = a_mesh->mNumFaces;
	const uint numIndices = numFaces * 3;
	bool hasTextureCoords = a_mesh->HasTextureCoords(0);
	bool hasTangentsAndBitangents = a_mesh->HasTangentsAndBitangents();
	uint baseVertex = m_vertices.size();

	m_indices.reserve(m_indices.size() + numIndices);
	m_vertices.reserve(m_vertices.size() + numVertices);

	for (uint i = 0; i < numFaces; ++i)
	{
		const aiFace& face = a_mesh->mFaces[i];
		m_indices.push_back(face.mIndices[0] + baseVertex);
		m_indices.push_back(face.mIndices[1] + baseVertex);
		m_indices.push_back(face.mIndices[2] + baseVertex);
	}

	for (uint i = 0; i < numVertices; ++i)
	{
		m_vertices.push_back(Vertex(
			(a_vertexTransform != NULL) ? (*a_vertexTransform) * a_mesh->mVertices[i] : a_mesh->mVertices[i],
			hasTextureCoords ? a_mesh->mTextureCoords[0][i] : aiVector3D(0.0f),
			a_mesh->mNormals[i],
			hasTangentsAndBitangents ? a_mesh->mTangents[i] : aiVector3D(0.0f),
			hasTangentsAndBitangents ? a_mesh->mBitangents[i] : aiVector3D(0.0f),
			a_mesh->mMaterialIndex));
	}
}

uint Mesh::getByteSize()
{
	throw std::logic_error("The method or operation is not implemented.");
}

void Mesh::write(std::ostream& file)
{
	throw std::logic_error("The method or operation is not implemented.");
}
