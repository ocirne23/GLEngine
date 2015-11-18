#include "SceneProcessor.h"

#include "BuilderCore.h"
#include "Database/EAssetType.h"
#include "Database/AssetDatabase.h"
#include "Database/Assets/Scene.h"
#include "Utils/FileUtils.h"
#include "Utils/stb_image.h"
#include "Utils/TextureAtlas.h"
#include "Utils/writeVector.h"
#include "Utils/AtlasBuilder.h"

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <fstream>
#include <iostream>

BEGIN_UNNAMED_NAMESPACE()

struct vec4 { float x, y, z, w; };
struct vec3 { float x, y, z; };
struct vec2 { float x, y; };

struct Vertex
{
	Vertex() {};
	Vertex(aiVector3D& a_position, aiVector3D& a_texcoords, aiVector3D& a_normal, aiVector3D& a_tangents, aiVector3D& a_bitangents, uint a_materialID)
		: position((vec3&) a_position), texcoords((vec2&) a_texcoords), normal((vec3&) a_normal)
		, tangents((vec3&) a_tangents), bitangents((vec3&) a_bitangents), materialID(a_materialID)
	{}
	vec3 position;
	vec2 texcoords;
	vec3 normal;
	vec3 tangents;
	vec3 bitangents;
	uint materialID;
};

struct GLSLMaterial
{
	vec4 diffuseTexMapping;
	vec4 normalTexMapping;
	int diffuseAtlasNr = -1;
	int normalAtlasNr = -1;
	int padding = -1;
	int padding2 = -1;
};

struct MeshData
{
	std::vector<std::string> meshes;
	aiMatrix3x3 transform;
	uint indiceCounter = 0;
	uint vertexCounter = 0;
	std::vector<uint> indices;
	std::vector<Vertex> vertices;
};

struct MeshEntry
{
	uint numOccurences = 0;
	aiMatrix3x3 transform;
};

void countMeshes(std::vector<uint>& a_meshCountList, const aiNode* a_node)
{
	for (uint i = 0; i < a_node->mNumMeshes; ++i)
		a_meshCountList[a_node->mMeshes[i]]++;

	uint numChildren = a_node->mNumChildren;
	for (uint i = 0; i < numChildren; ++i)
		countMeshes(a_meshCountList, a_node->mChildren[i]);
}

/* Get the number of occurrences for every mesh */
std::vector<uint> countMeshes(const aiScene* a_assimpScene)
{
	std::vector<uint> meshCountList(a_assimpScene->mNumMeshes);
	countMeshes(meshCountList, a_assimpScene->mRootNode);
	return meshCountList;
}

void countMeshes(std::vector<MeshEntry>& a_meshEntryList, aiNode* a_node, aiMatrix3x3 a_parentTransform)
{
	const aiMatrix4x4& trans4d = a_node->mTransformation;
	aiMatrix3x3 transform = a_parentTransform * aiMatrix3x3(trans4d.a1, trans4d.a2, trans4d.a3,
															trans4d.b1, trans4d.b2, trans4d.b3,
															trans4d.c1, trans4d.c2, trans4d.c3);

	uint numMeshes = a_node->mNumMeshes;
	for (uint i = 0; i < numMeshes; ++i)
	{
		uint meshIdx = a_node->mMeshes[i];
		a_meshEntryList[meshIdx].numOccurences++;
		if (a_meshEntryList[meshIdx].numOccurences == 1)
			a_meshEntryList[meshIdx].transform = transform;
	}
	uint numChildren = a_node->mNumChildren;
	for (uint i = 0; i < numChildren; ++i)
		countMeshes(a_meshEntryList, a_node->mChildren[i], transform);
}

/* Get the transform of the first node using a mesh with the given ID, returns if a transform was found */
bool findAbsoluteTransformForMesh(uint a_meshID, const aiNode* a_node, aiMatrix3x3& a_parentTransformAndResult)
{
	const aiMatrix4x4& trans4d = a_node->mTransformation;
	const aiMatrix3x3 transform = a_parentTransformAndResult * aiMatrix3x3(trans4d.a1, trans4d.a2, trans4d.a3, 
	                                                                       trans4d.b1, trans4d.b2, trans4d.b3, 
	                                                                       trans4d.c1, trans4d.c2, trans4d.c3);
	uint numMeshes = a_node->mNumMeshes;
	for (uint i = 0; i < numMeshes; ++i)
		if (a_node->mMeshes[i] == a_meshID)
		{
			a_parentTransformAndResult = transform;
			return true;
		}

	uint numChildren = a_node->mNumChildren;
	for (uint i = 0; i < numChildren; ++i)
		if (findAbsoluteTransformForMesh(a_meshID, a_node->mChildren[i], a_parentTransformAndResult))
			return true;

	return false;
}

aiMatrix3x3 findAbsoluteTransformForMesh(const aiScene* a_assimpScene, uint a_meshId)
{
	const aiNode* rootNode = a_assimpScene->mRootNode;
	aiMatrix3x3 identityMatrix;
	bool result = findAbsoluteTransformForMesh(a_meshId, rootNode, identityMatrix);
	assert(result && "Transform for mesh was not found in scene");
	return identityMatrix;
}

void appendMesh(const aiScene* a_assimpScene, MeshData& a_meshData, uint a_meshID, bool a_writeAbsoluteTransform)
{
	aiMatrix3x3 transform = a_writeAbsoluteTransform ? findAbsoluteTransformForMesh(a_assimpScene, a_meshID) : aiMatrix3x3();
	const aiMesh* mesh = a_assimpScene->mMeshes[a_meshID];

	const uint numVertices = mesh->mNumVertices;
	const uint numFaces = mesh->mNumFaces;
	bool hasTextureCoords = mesh->HasTextureCoords(0);
	bool hasTangentsAndBitangents = mesh->HasTangentsAndBitangents();
	uint baseVertex = a_meshData.vertexCounter;

	for (uint i = 0; i < numFaces; ++i)
	{
		const aiFace& face = mesh->mFaces[i];
		a_meshData.indices[a_meshData.indiceCounter++] = face.mIndices[0] + baseVertex;
		a_meshData.indices[a_meshData.indiceCounter++] = face.mIndices[1] + baseVertex;
		a_meshData.indices[a_meshData.indiceCounter++] = face.mIndices[2] + baseVertex;
	}

	for (uint i = 0; i < numVertices; ++i)
	{
		a_meshData.vertices[a_meshData.vertexCounter++] = Vertex(
				a_writeAbsoluteTransform ? transform * mesh->mVertices[i] : mesh->mVertices[i],
				hasTextureCoords ? mesh->mTextureCoords[0][i] : aiVector3D(0.0f),
				mesh->mNormals[i],
				hasTangentsAndBitangents ? mesh->mTangents[i] : aiVector3D(0.0f),
				hasTangentsAndBitangents ? mesh->mBitangents[i] : aiVector3D(0.0f),
				mesh->mMaterialIndex);
	}
}

MeshData getSingleOccurrenceMergedMesh(const aiScene* a_scene, const std::vector<uint>& a_meshCountList)
{
	MeshData mergedMeshData;
	uint numIndices = 0;
	uint numVertices = 0;
	for (uint i = 0; i < a_meshCountList.size(); ++i)
	{
		if (a_meshCountList[i] == 1)
		{
			const aiMesh* mesh = a_scene->mMeshes[i];
			numIndices += mesh->mNumVertices;
			numVertices += mesh->mNumFaces * 3;
		}
	}
	mergedMeshData.indices.resize(numIndices);
	mergedMeshData.vertices.resize(numVertices);

	for (uint i = 0; i < a_meshCountList.size(); ++i)
		if (a_meshCountList[i] == 1)
			appendMesh(a_scene, mergedMeshData, i, true);

	return mergedMeshData;
}

std::vector<MeshData> getMultiOccurenceMeshes(const aiScene* a_scene, const std::vector<uint>& a_meshCountList)
{
	std::vector<MeshData> seperateMeshes;
	for (uint i = 0; i < a_meshCountList.size(); ++i)
	{
		if (a_meshCountList[i] > 1)
		{
			MeshData mesh;
			mesh.indices.resize(a_scene->mMeshes[i]->mNumVertices);
			mesh.vertices.resize(a_scene->mMeshes[i]->mNumFaces * 3);
			appendMesh(a_scene, mesh, i, false);
			seperateMeshes.push_back(mesh);
		}
	}
	return seperateMeshes;
}

vec4 getTextureMapping(uint a_atlasWidth, uint a_atlasHeight, const TextureAtlas::AtlasRegion& a_reg)
{
	const float wScale = a_reg.width / (float) a_atlasWidth;
	const float hScale = a_reg.height / (float) a_atlasHeight;
	const float xOffset = a_reg.x / (float) a_atlasWidth;
	const float yOffset = a_reg.y / (float) a_atlasHeight;
	return {xOffset, yOffset, wScale, hScale};
}

// int: atlasIdx, vec4: mapping
std::pair<int, vec4> getGLSLTexLocation(const std::vector<AtlasTextureInfo>& a_textures, const std::string& a_path, const std::vector<TextureAtlas*>& a_atlases)
{
	for (const AtlasTextureInfo& tex : a_textures)
	{
		if (tex.textureInfo.filePath == a_path)
		{
			const TextureAtlas* atlas = a_atlases[tex.atlasInfo.atlasIdx];
			vec4 mapping = getTextureMapping(atlas->m_width, atlas->m_height, tex.atlasInfo.region);
			return {tex.atlasInfo.atlasIdx, mapping};
		}
	}
	return {-1, {0.0f, 0.0f, 0.0f, 0.0f}};
}
AtlasTextureInfo::AtlasInfo getAtlasInfo(const std::vector<AtlasTextureInfo>& a_textures, const std::string& a_path)
{
	for (const AtlasTextureInfo& tex : a_textures)
	{
		if (tex.textureInfo.filePath == a_path)
			return tex.atlasInfo;
	}
	assert(false);
	return AtlasTextureInfo::AtlasInfo();
}

std::vector<GLSLMaterial> getMaterials(const aiScene* a_scene, const std::vector<AtlasTextureInfo>& a_textures, const std::vector<TextureAtlas*>& a_atlases)
{
	std::vector<GLSLMaterial> materials;
	for (uint i = 0; i < a_scene->mNumMaterials; i++)
	{
		GLSLMaterial material;
		aiString path;
		const aiMaterial* assimpMat = a_scene->mMaterials[i];
		if (assimpMat->GetTextureCount(aiTextureType_DIFFUSE))
		{
			assimpMat->GetTexture(aiTextureType_DIFFUSE, 0, &path);
			std::pair<int, vec4> loc = getGLSLTexLocation(a_textures, path.C_Str(), a_atlases);
			material.diffuseAtlasNr = loc.first;
			material.diffuseTexMapping = loc.second;
		}
		if (assimpMat->GetTextureCount(aiTextureType_NORMALS))
		{
			assimpMat->GetTexture(aiTextureType_NORMALS, 0, &path);
			std::pair<int, vec4> loc = getGLSLTexLocation(a_textures, path.C_Str(), a_atlases);
			material.normalAtlasNr = loc.first;
			material.normalTexMapping = loc.second;
		}
		materials.push_back(material);
	}
	return materials;
}

END_UNNAMED_NAMESPACE()

void SceneProcessor::process(const char* a_inResourcePath, AssetDatabase& a_assetDatabase, std::vector<std::string>& a_rebuildOnFileModificationList)
{
	const uint flags = 0
		| aiPostProcessSteps::aiProcess_Triangulate
		| aiPostProcessSteps::aiProcess_CalcTangentSpace
		| aiPostProcessSteps::aiProcess_GenNormals
		| aiPostProcessSteps::aiProcess_RemoveRedundantMaterials
		| aiPostProcessSteps::aiProcess_FlipUVs; // Flip uv's because OpenGL

	const aiScene* assimpScene = aiImportFile(a_inResourcePath, flags);
	printf("building: %s\n", a_inResourcePath);

	// We want to merge all meshes that occur only once in one big mesh and keep the others in their own mesh
	std::vector<uint> meshCountList = countMeshes(assimpScene);
	MeshData mergedMeshData = getSingleOccurrenceMergedMesh(assimpScene, meshCountList);
	std::vector<MeshData> seperateMeshes = getMultiOccurenceMeshes(assimpScene, meshCountList);

	std::vector<AtlasTextureInfo> textureInfoList = AtlasBuilder::getTextures(assimpScene);
	std::vector<TextureAtlas*> atlases = AtlasBuilder::fitMaterials(textureInfoList);
	std::vector<GLSLMaterial> glslMaterials = getMaterials(assimpScene, textureInfoList, atlases);
	AtlasBuilder::fillAtlases(atlases, textureInfoList, FileUtils::getFolderPathForFile(a_inResourcePath));
	
	print("Built scene: %s\n", a_inResourcePath);
	print("NumTextures: %i\n", textureInfoList.size());
	print("NumAtlases: %i\n", atlases.size());
	print("NumMaterials: %i\n", glslMaterials.size());
	// TODO: write stuff

	for (TextureAtlas* atlas : atlases)
		delete atlas;
	atlases.clear();
	aiReleaseImport(assimpScene);
}



bool SceneProcessor::process(const char* a_inResourcePath, const char* a_outResourcePath, std::vector<std::string>& a_rebuildDependencies)
{
	AssetDatabase db;
	process(a_inResourcePath, db, a_rebuildDependencies);
	return true;

	const std::string inResourcePathStr(a_inResourcePath);
	const std::string outResourcePathstr(a_outResourcePath);

	const uint flags = 0
		| aiPostProcessSteps::aiProcess_Triangulate
		| aiPostProcessSteps::aiProcess_CalcTangentSpace
		| aiPostProcessSteps::aiProcess_GenNormals
		| aiPostProcessSteps::aiProcess_RemoveRedundantMaterials
		| aiPostProcessSteps::aiProcess_FlipUVs; // Flip uv's because OpenGL

	const aiScene* scene = aiImportFile(a_inResourcePath, flags);

	printf("building: %s \n", a_inResourcePath);

	if (!scene)
	{
		printf("Error parsing scene '%s' : %s\n", a_inResourcePath, aiGetErrorString());
		return false;
	}

	std::ofstream file(a_outResourcePath, std::ios::out | std::ios::binary);
	assert(file.is_open());

	file.close();

	aiReleaseImport(scene);
	scene = NULL;

	return true;
}