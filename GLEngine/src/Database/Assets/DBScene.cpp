#include "Database/Assets/DBScene.h"

#include "Database/Utils/AtlasBuilder.h"
#include "EASTL/string.h"
#include "Utils/FileUtils.h"

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

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


DBScene::DBScene(const eastl::string& a_sceneFilePath)
{
	const uint flags = 0
	// Required flags
	| aiPostProcessSteps::aiProcess_Triangulate
	| aiPostProcessSteps::aiProcess_CalcTangentSpace
	| aiPostProcessSteps::aiProcess_GenNormals
	| aiPostProcessSteps::aiProcess_FlipUVs // Flip uv's because OpenGL
	// Optimalizations
	//| aiPostProcessSteps::aiProcess_RemoveRedundantMaterials // We might want to swap in unused materials at runtime
	| aiPostProcessSteps::aiProcess_ImproveCacheLocality
	| aiPostProcessSteps::aiProcess_OptimizeMeshes
	| aiPostProcessSteps::aiProcess_OptimizeGraph
	//| aiPostProcessSteps::aiProcess_PreTransformVertices // Removes animations
	| aiPostProcessSteps::aiProcess_FindDegenerates
	| aiPostProcessSteps::aiProcess_GenUVCoords
	| aiPostProcessSteps::aiProcess_JoinIdenticalVertices
	| 0;

	const aiScene* assimpScene = aiImportFile(a_sceneFilePath.c_str(), flags);
	const eastl::string baseAssetPath = FileUtils::getFolderPathForFile(a_sceneFilePath);

	processNodes(m_nodes, assimpScene->mRootNode, 0);

	m_meshes.resize(assimpScene->mNumMeshes);
	for (uint i = 0; i < assimpScene->mNumMeshes; ++i)
		m_meshes[i] = DBMesh(*assimpScene->mMeshes[i]);
	
	m_materials.resize(assimpScene->mNumMaterials);
	for (uint i = 0; i < assimpScene->mNumMaterials; ++i)
		m_materials[i] = DBMaterial(*assimpScene->mMaterials[i]);

	aiReleaseImport(assimpScene);

	m_atlasTextures = AtlasBuilder::createAtlases(m_materials, baseAssetPath);
}

void DBScene::mergeMeshes()
{
	DBMesh mergedMesh;
	mergeMeshes(mergedMesh, m_nodes[0], glm::mat4(1));

	m_nodes.resize(1);  // Clear all nodes except the root node
	m_meshes.resize(1);
	m_nodes[0].clearChildren();
	m_nodes[0].clearMeshes();
	m_nodes[0].addMesh(0);
	m_meshes[0] = mergedMesh;
}

void DBScene::mergeMeshes(DBMesh& mergedMesh, DBNode& node, const glm::mat4& parentTransform)
{
	glm::mat4 transform = parentTransform * node.getTransform();
	for (uint meshIdx : node.getMeshIndices())
		mergedMesh.merge(m_meshes[meshIdx], transform);
	for (uint childIdx : node.getChildIndices())
		mergeMeshes(mergedMesh, m_nodes[childIdx], transform);
}

uint64 DBScene::getByteSize() const
{
	uint64 totalSize = 0;
	
	totalSize += AssetDatabaseEntry::getValWriteSize(uint(m_nodes.size()));
	for (uint i = 0; i < m_nodes.size(); ++i)
		totalSize += m_nodes[i].getByteSize();
	
	totalSize += AssetDatabaseEntry::getValWriteSize(uint(m_meshes.size()));
	for (uint i = 0; i < m_meshes.size(); ++i)
		totalSize += m_meshes[i].getByteSize();
	
	totalSize += AssetDatabaseEntry::getValWriteSize(uint(m_materials.size()));
	for (uint i = 0; i < m_materials.size(); ++i)
		totalSize += m_materials[i].getByteSize();
	
	totalSize += AssetDatabaseEntry::getValWriteSize(uint(m_atlasTextures.size()));
	for (uint i = 0; i < m_atlasTextures.size(); ++i)
		totalSize += m_atlasTextures[i].getByteSize();
	
	return totalSize;
}

void DBScene::write(AssetDatabaseEntry& entry)
{
	entry.writeVal(uint(m_nodes.size()));
	for (uint i = 0; i < m_nodes.size(); ++i)
		m_nodes[i].write(entry);
	
	entry.writeVal(uint(m_meshes.size()));
	for (uint i = 0; i < m_meshes.size(); ++i)
		m_meshes[i].write(entry);
	
	entry.writeVal(uint(m_materials.size()));
	for (uint i = 0; i < m_materials.size(); ++i)
		m_materials[i].write(entry);
	
	entry.writeVal(uint(m_atlasTextures.size()));
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