#include "Database/Assets/DBMesh.h"

#include <assimp/scene.h>

DBMesh::DBMesh(const aiMesh& a_assimpMesh)
{
	m_name = a_assimpMesh.mName.C_Str();
	
	const uint numVertices = a_assimpMesh.mNumVertices;
	const uint numFaces = a_assimpMesh.mNumFaces;
	const uint numIndices = numFaces * 3;
	const bool hasTextureCoords = a_assimpMesh.HasTextureCoords(0);
	const bool hasTangentsAndBitangents = a_assimpMesh.HasTangentsAndBitangents();

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
		v.position =                      *rcast<glm::vec3*>(&a_assimpMesh.mVertices[i]);
		v.texcoords = (hasTextureCoords ? *rcast<glm::vec2*>(&a_assimpMesh.mTextureCoords[0][i]) : glm::vec2(0));
		v.normal =                        *rcast<glm::vec3*>(&a_assimpMesh.mNormals[i]);
		if (hasTangentsAndBitangents)
		{
			glm::vec3 tangent = *rcast<glm::vec3*>(&a_assimpMesh.mTangents[i]);
			glm::vec3 bitangent = *rcast<glm::vec3*>(&a_assimpMesh.mBitangents[i]);
			float handedness = glm::dot(v.normal, glm::cross(tangent, bitangent)) >= 0.0f ? 1.0f : -1.0f;
			v.tangents = glm::vec4(tangent, handedness);
		}
		v.materialID = a_assimpMesh.mMaterialIndex;

		m_boundsMin = glm::min(m_boundsMin, v.position);
		m_boundsMax = glm::max(m_boundsMax, v.position);
	}
	//print("mesh min: %f, %f, %f max: %f, %f, %f\n", m_boundsMin.x, m_boundsMin.y, m_boundsMin.z, m_boundsMax.x, m_boundsMax.y, m_boundsMax.z);
}

void DBMesh::merge(const DBMesh& a_mesh, const glm::mat4& a_transform)
{
	m_name += ":MERGED:" + a_mesh.getName();
	const uint numIndices = uint(a_mesh.getIndices().size());
	const uint numVertices = uint(a_mesh.getVertices().size());
	const uint baseIdx = uint(m_indices.size());
	const uint baseVertex = uint(m_vertices.size());
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

		m_boundsMin = glm::min(m_boundsMin, v.position);
		m_boundsMax = glm::max(m_boundsMax, v.position);
	}	
}

uint64 DBMesh::getByteSize() const
{
	uint64 totalSize = 0;
	totalSize += AssetDatabaseEntry::getStringWriteSize(m_name);
	totalSize += AssetDatabaseEntry::getVectorWriteSize(m_vertices);
	totalSize += AssetDatabaseEntry::getVectorWriteSize(m_indices);
	totalSize += AssetDatabaseEntry::getValWriteSize(m_boundsMin);
	totalSize += AssetDatabaseEntry::getValWriteSize(m_boundsMax);
	return totalSize;
}

void DBMesh::write(AssetDatabaseEntry& entry)
{
	entry.writeString(m_name);
	entry.writeVector(m_vertices);
	entry.writeVector(m_indices);
	entry.writeVal(m_boundsMin);
	entry.writeVal(m_boundsMax);
}

void DBMesh::read(AssetDatabaseEntry& entry)
{
	entry.readString(m_name);
	entry.readVector(m_vertices);
	entry.readVector(m_indices);
	entry.readVal(m_boundsMin);
	entry.readVal(m_boundsMax);
}