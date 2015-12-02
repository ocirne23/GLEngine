#include "Database/Assets/Scene/DBNode.h"

#include <assimp/scene.h>

DBNode::DBNode(const aiNode& a_assimpNode, uint a_parentIdx)
{
	m_name = a_assimpNode.mName.C_Str();
	m_parentIdx = a_parentIdx;
	m_childIndices.reserve(a_assimpNode.mNumChildren);

	m_meshIndices.reserve(a_assimpNode.mNumMeshes);
	for (uint i = 0; i < a_assimpNode.mNumMeshes; ++i)
		m_meshIndices.push_back(a_assimpNode.mMeshes[i]);
}

uint64 DBNode::getByteSize() const
{
	uint64 totalSize = 0;
	totalSize += AssetDatabaseEntry::getStringWriteSize(m_name);
	totalSize += AssetDatabaseEntry::getArrayWriteSize(m_transform, ARRAY_SIZE(m_transform));
	totalSize += sizeof(m_parentIdx);
	totalSize += AssetDatabaseEntry::getVectorWriteSize(m_childIndices);
	totalSize += AssetDatabaseEntry::getVectorWriteSize(m_meshIndices);
	return totalSize;
}

void DBNode::write(AssetDatabaseEntry& entry)
{
	entry.writeString(m_name);
	entry.writeArray(m_transform, ARRAY_SIZE(m_transform));
	entry.writeVal(m_parentIdx);
	entry.writeVector(m_childIndices);
	entry.writeVector(m_meshIndices);
}

void DBNode::read(AssetDatabaseEntry& entry)
{
}

void DBNode::addChild(uint a_childIdx)
{
	m_childIndices.push_back(a_childIdx);
}