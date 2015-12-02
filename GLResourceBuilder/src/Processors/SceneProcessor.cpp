#include "Processors/SceneProcessor.h"

#include "BuilderCore.h"
#include "Database/EAssetType.h"
#include "Database/AssetDatabase.h"
#include "Database/Assets/Scene/DBScene.h"
#include "Database/Assets/Scene/DBMaterial.h"
#include "Database/Assets/Scene/DBMesh.h"

#include "Utils/FileUtils.h"
#include "Utils/stb_image.h"
#include "Utils/AtlasBuilder.h"

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <fstream>
#include <iostream>

BEGIN_UNNAMED_NAMESPACE()

const std::string TEXTURE_SUB_PATH_NAME = "/TEXTURES/";
const std::string MATERIAL_SUB_PATH_NAME = "/MATERIALS/";
const std::string MESH_SUB_PATH_NAME = "/MESHES/";

std::string getTextureDBPath(const std::string& a_baseScenePath, const std::string& a_textureName)
{
	return a_baseScenePath + TEXTURE_SUB_PATH_NAME + a_textureName.c_str();
}

std::string getMaterialDBPath(const std::string& a_baseScenePath, uint materialIdx)
{
	return a_baseScenePath + MATERIAL_SUB_PATH_NAME + "#" + std::to_string(materialIdx);
}

std::string getMeshDBPath(const std::string& a_baseScenePath, const std::string& a_meshName)
{
	return a_baseScenePath + MESH_SUB_PATH_NAME + a_meshName;
}

// !Meshes stuff
/*
vec4 getTextureMapping(uint a_atlasWidth, uint a_atlasHeight, const AtlasPosition& a_atlasPos)
{
	const float xOffset = a_atlasPos.xPos / (float) a_atlasWidth;
	const float yOffset = a_atlasPos.yPos / (float) a_atlasHeight;
	const float width = a_atlasPos.width / (float) a_atlasWidth;
	const float height = a_atlasPos.height / (float) a_atlasHeight;
	return {xOffset, yOffset, width, height};
}
*/
/*
void addTextureInfo(AssetDatabase& a_assetDatabase, aiTextureType a_textureType, const aiMaterial* a_material, const std::string& a_baseScenePath)
{
	std::string texturePath;
	if (a_material->GetTextureCount(a_textureType))
	{
		aiString path;
		a_material->GetTexture(a_textureType, 0, &path);
		if (path.data[0] == '*') //TODO
		{
			print("EMBEDDED TEXTURE: %s\n", path.C_Str());
			assert(false);
			return;
		}
		texturePath = a_baseScenePath + TEXTURE_SUB_PATH_NAME + path.C_Str();
	}
	else
	{
		assert(false && "Material doesnt have a texture of the type");
		return;
	}

	if (a_assetDatabase.getAsset(texturePath.c_str()) != NULL)
		return;

	// If texture doesnt exist, add to db
	int width, height, numComp;
	int result = stbi_info(texturePath.c_str(), &width, &height, &numComp);
	if (result)
	{
		AtlasTexture* tex = new AtlasTexture();
		tex->m_filePath = texturePath;
		tex->m_width = width;
		tex->m_height = height;
		tex->m_numComp = numComp;
		a_assetDatabase.addAsset(texturePath.c_str(), tex);
	}
	else
		assert(false && "Failed to load texture");
}

void addTexturesToDatabase(const aiScene* a_scene, const std::string& a_baseScenePath, AssetDatabase& a_assetDatabase)
{
	print("NUM EMBEDDED TEXTURES: %i\n", a_scene->mNumTextures);
	for (uint i = 0; i < a_scene->mNumMaterials; ++i)
	{
		const aiMaterial* material = a_scene->mMaterials[i];

		if (material->GetTextureCount(aiTextureType_DIFFUSE))
			addTextureInfo(a_assetDatabase, aiTextureType_DIFFUSE, material, a_baseScenePath);
		else
			print("No diffuse texture for material found\n");

		if (material->GetTextureCount(aiTextureType_NORMALS))
			addTextureInfo(a_assetDatabase, aiTextureType_NORMALS, material, a_baseScenePath);
		else if (material->GetTextureCount(aiTextureType_HEIGHT))
			addTextureInfo(a_assetDatabase, aiTextureType_HEIGHT, material, a_baseScenePath);  // .obj files have their normals map under aiTextureType_HEIGHT
	}
}

void addMeshesToDatabase(const aiScene* a_scene, const std::string& a_baseScenePath, AssetDatabase& a_assetDatabase)
{
	for (uint i = 0; i < a_scene->mNumMeshes; ++i)
	{
		const std::string dbPath = getMeshDBPath(a_baseScenePath, a_scene->mMeshes[i]->mName.C_Str());
		a_assetDatabase.addAsset(dbPath, new Mesh(*a_scene->mMeshes[i]));
	}
}

void addMaterialsToDatabase(const aiScene* a_scene, const std::string& a_baseScenePath, AssetDatabase& a_assetDatabase)
{
	for (uint i = 0; i < a_scene->mNumMaterials; ++i)
	{
		std::string dbPath = getMaterialDBPath(a_baseScenePath, i);
		a_assetDatabase.addAsset(dbPath, new Material(*a_scene->mMaterials[i]));
	}
}

void addNode(Scene* a_scene, aiNode* a_node, uint a_parentIdx)
{
	uint idx = a_scene->addNode(a_node->mName.C_Str(), a_parentIdx);
	for (uint i = 0; i < a_node->mNumChildren; i++)
		addNode(a_scene, a_node->mChildren[i], idx);
	for (uint i = 0; i < a_node->mNumMeshes; ++i)
		a_scene->addMesh(idx, a_node->mMeshes[i]);
}


void addScenegraphToDatabase(const aiScene* a_scene, const std::string& a_baseScenePath, AssetDatabase& a_assetDatabase)
{
	aiNode* rootNode = a_scene->mRootNode;
	assert(rootNode);
	Scene* scene = new Scene();
	addNode(scene, rootNode, 0);
}
*/
END_UNNAMED_NAMESPACE()

bool SceneProcessor::process(const eastl::string& a_inResourcePath, AssetDatabase& a_assetDatabase)
{
	const uint flags = 0
		| aiPostProcessSteps::aiProcess_Triangulate
		| aiPostProcessSteps::aiProcess_CalcTangentSpace
		| aiPostProcessSteps::aiProcess_GenNormals
		| aiPostProcessSteps::aiProcess_RemoveRedundantMaterials
		| aiPostProcessSteps::aiProcess_FlipUVs; // Flip uv's because OpenGL

	const aiScene* assimpScene = aiImportFile(a_inResourcePath.c_str(), flags);
	printf("building: %s\n", a_inResourcePath.c_str());
	
	a_assetDatabase.addAsset(a_inResourcePath, new DBScene(*assimpScene, FileUtils::getFolderPathForFile(a_inResourcePath)));

	aiReleaseImport(assimpScene);
	return true;
}