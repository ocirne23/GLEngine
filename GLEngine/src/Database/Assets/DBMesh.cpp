#include "Database/Assets/DBMesh.h"

#include <assimp/scene.h>

DBMesh::DBMesh(const aiMesh& a_assimpMesh)
{
	m_name = a_assimpMesh.mName.C_Str();
	
	const uint numVertices = a_assimpMesh.mNumVertices;
	const uint numFaces = a_assimpMesh.mNumFaces;
	const uint numIndices = numFaces * 3;
	bool hasTextureCoords = a_assimpMesh.HasTextureCoords(0);
	//bool hasTangentsAndBitangents = a_assimpMesh.HasTangentsAndBitangents();

	m_indices.resize(numIndices);
	m_vertices.resize(numVertices);

	int indiceCounter = 0;
	for (uint i = 0; i < numFaces; ++i)
	{
		const aiFace& face = a_assimpMesh.mFaces[i];
		m_indices[indiceCounter++] = face.mIndices[0];
		m_indices[indiceCounter++] = face.mIndices[1];
		m_indices[indiceCounter++] = face.mIndices[2];
	}

	for (uint i = 0; i < numVertices; ++i)
	{
		Vertex& v = m_vertices[i];
		v.position =                               *reinterpret_cast<glm::vec3*>(&a_assimpMesh.mVertices[i]);
		v.texcoords =          (hasTextureCoords ? *reinterpret_cast<glm::vec2*>(&a_assimpMesh.mTextureCoords[0][i]) : glm::vec2(0));
		v.normal =                                (*reinterpret_cast<glm::vec3*>(&a_assimpMesh.mNormals[i]));
		//v.tangents =   (hasTangentsAndBitangents ? *reinterpret_cast<glm::vec3*>(&a_assimpMesh.mTangents[i]) : glm::vec3(0));
		//v.bitangents = (hasTangentsAndBitangents ? *reinterpret_cast<glm::vec3*>(&a_assimpMesh.mBitangents[i]) : glm::vec3(0));
		v.materialID = a_assimpMesh.mMaterialIndex;
	}
}

void DBMesh::merge(const DBMesh& a_mesh, const glm::mat4& a_transform)
{
	m_name += ":MERGED:" + a_mesh.getName();
	uint numIndices = a_mesh.getIndices().size();
	uint numVertices = a_mesh.getVertices().size();
	uint baseIdx = m_indices.size();
	uint baseVertex = m_vertices.size();
	m_indices.resize(m_indices.size() + numIndices);
	m_vertices.resize(m_vertices.size() + numVertices);

	const eastl::vector<uint>& indices = a_mesh.getIndices();
	for (uint i = 0; i < numIndices; ++i)
		m_indices[baseIdx + i] = indices[i] + baseVertex;

	glm::mat4 normalTransform = a_transform;
	normalTransform[3] = glm::vec4(0, 0, 0, 1); // We do not want to translate normals

	const eastl::vector<Vertex>& vertices = a_mesh.getVertices();
	for (uint i = 0; i < numVertices; ++i)
	{
		Vertex v = vertices[i];
		v.position = glm::vec3(a_transform * glm::vec4(v.position, 1.0));
		v.normal = glm::normalize(glm::vec3(normalTransform * glm::vec4(v.normal, 1.0)));
		m_vertices[baseVertex + i] = v;
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