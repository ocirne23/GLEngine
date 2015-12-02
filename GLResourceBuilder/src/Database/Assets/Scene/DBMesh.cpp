#include "Database/Assets/Scene/DBMesh.h"

#include <assimp/scene.h>

DBMesh::DBMesh(const aiMesh& a_assimpMesh)
{
	m_name = a_assimpMesh.mName.C_Str();
	
	const uint numVertices = a_assimpMesh.mNumVertices;
	const uint numFaces = a_assimpMesh.mNumFaces;
	const uint numIndices = numFaces * 3;
	bool hasTextureCoords = a_assimpMesh.HasTextureCoords(0);
	bool hasTangentsAndBitangents = a_assimpMesh.HasTangentsAndBitangents();
	uint baseVertex = (uint) m_vertices.size();

	m_indices.reserve(m_indices.size() + numIndices);
	m_vertices.resize(m_vertices.size() + numVertices);

	for (uint i = 0; i < numFaces; ++i)
	{
		const aiFace& face = a_assimpMesh.mFaces[i];
		m_indices.push_back(face.mIndices[0] + baseVertex);
		m_indices.push_back(face.mIndices[1] + baseVertex);
		m_indices.push_back(face.mIndices[2] + baseVertex);
	}

	uint vertexStartIdx = m_vertices.size();
	for (uint i = 0; i < numVertices; ++i)
	{
		m_vertices[vertexStartIdx + i] = Vertex(
			a_assimpMesh.mVertices[i],
			hasTextureCoords ? a_assimpMesh.mTextureCoords[0][i] : aiVector3D(0.0f, 0.0f, 0.0f),
			a_assimpMesh.mNormals[i],
			hasTangentsAndBitangents ? a_assimpMesh.mTangents[i] : aiVector3D(0.0f, 0.0f, 0.0f),
			hasTangentsAndBitangents ? a_assimpMesh.mBitangents[i] : aiVector3D(0.0f, 0.0f, 0.0f),
			a_assimpMesh.mMaterialIndex);
	}
}

uint64 DBMesh::getByteSize() const
{
	uint64 totalSize = 0;
	totalSize += AssetDatabaseEntry::getStringWriteSize(m_name);
	totalSize += AssetDatabaseEntry::getVectorWriteSize(m_vertices);
	totalSize += AssetDatabaseEntry::getVectorWriteSize(m_indices);
	return totalSize;
}

void DBMesh::write(AssetDatabaseEntry& entry)
{
	entry.writeString(m_name);
	entry.writeVector(m_vertices);
	entry.writeVector(m_indices);
}

void DBMesh::read(AssetDatabaseEntry& entry)
{
}