#include "Database/Assets/DBMesh.h"

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

	uint vertexStartIdx = m_vertices.size();
	uint indiceStartIdx = m_indices.size();
	m_indices.resize(indiceStartIdx + numIndices);
	m_vertices.resize(vertexStartIdx + numVertices);

	for (uint i = 0; i < numFaces; ++i)
	{
		const aiFace& face = a_assimpMesh.mFaces[i];
		m_indices[indiceStartIdx++] = face.mIndices[0] + baseVertex;
		m_indices[indiceStartIdx++] = face.mIndices[1] + baseVertex;
		m_indices[indiceStartIdx++] = face.mIndices[2] + baseVertex;
	}

	for (uint i = 0; i < numVertices; ++i)
	{
		Vertex& v = m_vertices[vertexStartIdx + i];
		v.position =                               *reinterpret_cast<glm::vec3*>(&a_assimpMesh.mVertices[i]);
		v.texcoords =          (hasTextureCoords ? *reinterpret_cast<glm::vec2*>(&a_assimpMesh.mTextureCoords[0][i]) : glm::vec2(0));
		v.normal =                                 *reinterpret_cast<glm::vec3*>(&a_assimpMesh.mNormals[i]);
		v.tangents =   (hasTangentsAndBitangents ? *reinterpret_cast<glm::vec3*>(&a_assimpMesh.mTangents[i]) : glm::vec3(0));
		v.bitangents = (hasTangentsAndBitangents ? *reinterpret_cast<glm::vec3*>(&a_assimpMesh.mBitangents[i]) : glm::vec3(0));
		v.materialID = a_assimpMesh.mMaterialIndex;
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
	entry.readString(m_name);
	entry.readVector(m_vertices);
	entry.readVector(m_indices);
}