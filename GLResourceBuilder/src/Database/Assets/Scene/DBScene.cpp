#include "Database/Assets/Scene/DBScene.h"

#include "Utils/AtlasBuilder.h"

#include <assimp/scene.h>

DBScene::DBScene(const aiScene& a_assimpScene, const std::string& a_baseAssetPath)
{
	processNodes(a_assimpScene.mRootNode, 0);

	m_meshes.reserve(a_assimpScene.mNumMeshes);
	m_materials.reserve(a_assimpScene.mNumMaterials);
	
	for (uint i = 0; i < a_assimpScene.mNumMeshes; ++i)
		m_meshes.emplace_back(*a_assimpScene.mMeshes[i]);
	for (uint i = 0; i < a_assimpScene.mNumMaterials; ++i)
		m_materials.emplace_back(*a_assimpScene.mMaterials[i]);

	m_atlasTextures = AtlasBuilder::createAtlases(m_materials, a_baseAssetPath);
}

uint DBScene::processNodes(const aiNode* a_assimpNode, uint a_parentIdx)
{
	uint idx = (uint) m_nodes.size();
	m_nodes.emplace_back(*a_assimpNode, a_parentIdx);
	if (idx != a_parentIdx)
		m_nodes[a_parentIdx].addChild(idx);
	for (uint i = 0; i < a_assimpNode->mNumChildren; i++)
		processNodes(a_assimpNode->mChildren[i], idx);
	return idx;
}

uint64 DBScene::getByteSize() const
{
	uint64 totalSize = 0;
	
	totalSize += sizeof(uint); // m_nodes.size();
	for (uint i = 0; i < 2; ++i)
		totalSize += m_nodes[i].getByteSize();
	
	totalSize += sizeof(uint); // m_meshes.size();
	for (uint i = 0; i < m_meshes.size(); ++i)
		totalSize += m_meshes[i].getByteSize();
	
	totalSize += sizeof(uint); // m_materials.size();
	for (uint i = 0; i < m_materials.size(); ++i)
		totalSize += m_materials[i].getByteSize();
	
	
	totalSize += sizeof(uint); // m_atlasTextures.size();
	for (uint i = 0; i < m_atlasTextures.size(); ++i)
		totalSize += m_atlasTextures[i].getByteSize();
	
	print("Scene byte size: %i\n", totalSize);
	return totalSize;
}

void DBScene::write(AssetDatabaseEntry& entry)
{
	print("Writing scene\n");
	print("numNodes: %i\n", m_nodes.size());
	print("numMeshes: %i\n", m_meshes.size());
	print("numMaterials: %i\n", m_materials.size());
	print("numAtlasTextures: %i\n", m_atlasTextures.size());

	print("Writing nodes\n");
	entry.writeVal((uint) 2);
	for (uint i = 0; i < 2; ++i)
		m_nodes[i].write(entry);
	
	print("Writing meshes\n");
	entry.writeVal((uint) m_meshes.size());
	for (uint i = 0; i < m_meshes.size(); ++i)
		m_meshes[i].write(entry);
	
	print("Writing materials\n");
	entry.writeVal((uint) m_materials.size());
	for (uint i = 0; i < m_materials.size(); ++i)
		m_materials[i].write(entry);
	
	print("Writing atlas textures\n");
	entry.writeVal((uint) m_atlasTextures.size());
	for (uint i = 0; i < m_atlasTextures.size(); ++i)
		m_atlasTextures[i].write(entry);
}

void DBScene::read(AssetDatabaseEntry& entry)
{
}
