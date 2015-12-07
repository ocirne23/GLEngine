#include "Database/Assets/DBScene.h"

#include "Database/Utils/AtlasBuilder.h"
#include "EASTL/string.h"

#include <assimp/scene.h>

BEGIN_UNNAMED_NAMESPACE()

uint processNodes(eastl::vector<DBNode>& a_nodes, const aiNode* a_assimpNode, uint a_parentIdx)
{
	uint idx = (uint) a_nodes.size();
	a_nodes.push_back(DBNode(*a_assimpNode, a_parentIdx));
	if (idx != a_parentIdx)
		a_nodes[a_parentIdx].addChild(idx);
	for (uint i = 0; i < a_assimpNode->mNumChildren; i++)
		processNodes(a_nodes, a_assimpNode->mChildren[i], idx);
	return idx;
}

END_UNNAMED_NAMESPACE()

DBScene::DBScene(const aiScene& a_assimpScene, const eastl::string& a_baseAssetPath, bool a_invertNormals)
{
	processNodes(m_nodes, a_assimpScene.mRootNode, 0);

	m_meshes.resize(a_assimpScene.mNumMeshes);
	m_materials.resize(a_assimpScene.mNumMaterials);
	
	for (uint i = 0; i < a_assimpScene.mNumMeshes; ++i)
		m_meshes[i] = DBMesh(*a_assimpScene.mMeshes[i], a_invertNormals);
	for (uint i = 0; i < a_assimpScene.mNumMaterials; ++i)
		m_materials[i] = DBMaterial(*a_assimpScene.mMaterials[i]);

	m_atlasTextures = AtlasBuilder::createAtlases(m_materials, a_baseAssetPath);
}

uint64 DBScene::getByteSize() const
{
	uint64 totalSize = 0;
	
	totalSize += sizeof(uint); // m_nodes.size();
	for (uint i = 0; i < m_nodes.size(); ++i)
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
	
	return totalSize;
}

void DBScene::write(AssetDatabaseEntry& entry)
{
	entry.writeVal((uint) m_nodes.size());
	for (uint i = 0; i < m_nodes.size(); ++i)
		m_nodes[i].write(entry);
	
	entry.writeVal((uint) m_meshes.size());
	for (uint i = 0; i < m_meshes.size(); ++i)
		m_meshes[i].write(entry);
	
	entry.writeVal((uint) m_materials.size());
	for (uint i = 0; i < m_materials.size(); ++i)
		m_materials[i].write(entry);
	
	entry.writeVal((uint) m_atlasTextures.size());
	for (uint i = 0; i < m_atlasTextures.size(); ++i)
		m_atlasTextures[i].write(entry);
}

void DBScene::read(AssetDatabaseEntry& entry)
{
	uint numNodes = 0;
	entry.readVal(numNodes);
	m_nodes.resize(numNodes);
	for (uint i = 0; i < numNodes; ++i)
		m_nodes[i].read(entry);

	uint numMeshes = 0;
	entry.readVal(numMeshes);
	m_meshes.resize(numMeshes);
	for (uint i = 0; i < numMeshes; ++i)
		m_meshes[i].read(entry);

	uint numMaterials = 0;
	entry.readVal(numMaterials);
	m_materials.resize(numMaterials);
	for (uint i = 0; i < numMaterials; ++i)
		m_materials[i].read(entry);

	uint numAtlasTextures = 0;
	entry.readVal(numAtlasTextures);
	m_atlasTextures.resize(numAtlasTextures);
	for (uint i = 0; i < numAtlasTextures; ++i)
		m_atlasTextures[i].read(entry);
}