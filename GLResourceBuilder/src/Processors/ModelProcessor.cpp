#include "ModelProcessor.h"

#include "Core.h"

#include "EResourceType.h"
#include "Utils/FileUtils.h"
#include "Utils/stb_image.h"
#include "Utils/TextureAtlas.h"
#include "Utils/writeVector.h"

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <fstream>
#include <iostream>

BEGIN_UNNAMED_NAMESPACE()

enum { SUPPORT_NPOT = false, ATLAS_MAX_WIDTH = 4096, ATLAS_MAX_HEIGHT = 4096, ATLAS_NUM_MIPS = 4, ATLAS_NUM_COMPONENTS = 3, ATLAS_START_WIDTH = 256, ATLAS_START_HEIGHT = 256, ATLAS_INCREMENT = 256 };

// GLSL structs
struct vec4 { float x, y, z, w; };
struct vec3 { float x, y, z; };
struct vec2 { float x, y; };
struct MaterialProperty
{
	vec4 diffuseTexMapping;
	vec4 normalTexMapping;
	int diffuseAtlasNr = -1;
	int normalAtlasNr = -1;
	int padding = -1;
	int padding2 = -1;
};
// !GLSL structs
	
struct TextureInfo
{
	std::string filePath;
	int result = 0;
	int width = 0;
	int height = 0;
	int numComp = 0;

	TextureAtlas::AtlasRegion region;
	uint atlasIdx;
};

struct MaterialData
{
	MaterialData(uint a_materialID) : materialID(a_materialID) {}
	uint materialID = 0;
	int diffuseTextureInfoIndex = -1;
	int normalTextureInfoIndex = -1;
};

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

vec4 getTextureOffset(uint a_atlasWidth, uint a_atlasHeight, const TextureAtlas::AtlasRegion& a_reg)
{
	const int regionWidth = a_reg.width;
	const int regionHeight = a_reg.height;

	const int regionX = a_reg.x;
	const int regionY = a_reg.y;

	const float wScale = regionWidth / (float) a_atlasWidth;
	const float hScale = regionHeight / (float) a_atlasHeight;

	const float xOffset = regionX / (float) a_atlasWidth;
	const float yOffset = regionY / (float) a_atlasHeight;

	return {xOffset, yOffset, wScale, hScale};
}

void increaseAtlasesSize(std::vector<TextureInfo>& a_textureInfoList, std::vector<TextureAtlas*>& a_atlases)
{
	if (!a_atlases.size())
	{
		if (a_textureInfoList.size() == 1)
			a_atlases.push_back(new TextureAtlas(a_textureInfoList[0].width, a_textureInfoList[0].height, 4, ATLAS_NUM_MIPS));
		else
			a_atlases.push_back(new TextureAtlas(ATLAS_START_WIDTH, ATLAS_START_HEIGHT, 4, ATLAS_NUM_MIPS));
	}
	else
	{
		TextureAtlas* last = a_atlases.back();
		if (last->m_width >= ATLAS_MAX_WIDTH && last->m_height >= ATLAS_MAX_HEIGHT)
		{
			for (TextureAtlas* atlas : a_atlases)
				atlas->initialize(ATLAS_MAX_WIDTH / 2, ATLAS_MAX_HEIGHT / 2, ATLAS_NUM_COMPONENTS, ATLAS_NUM_MIPS);
			a_atlases.push_back(new TextureAtlas(ATLAS_MAX_WIDTH / 2, ATLAS_MAX_HEIGHT / 2, ATLAS_NUM_COMPONENTS, ATLAS_NUM_MIPS));
		}
		else
		{
			if (SUPPORT_NPOT)
			{
				int totalWidth = 0;
				int totalHeight = 0;
				for (TextureAtlas* atlas : a_atlases)
				{
					totalWidth += atlas->m_width;
					totalHeight += atlas->m_height;
				}
				int numAtlasses = (int) a_atlases.size();
				int avgWidth = totalWidth / numAtlasses;
				int avgHeight = totalHeight / numAtlasses;
				if (avgWidth < avgHeight)
					avgWidth += ATLAS_INCREMENT;
				else
					avgHeight += ATLAS_INCREMENT;

				// Make even
				if (avgHeight % 2 != 0)
					avgHeight++;
				if (avgWidth % 2 != 0)
					avgWidth++;

				avgWidth = std::min(avgWidth, (int) ATLAS_MAX_WIDTH);
				avgHeight = std::min(avgHeight, (int) ATLAS_MAX_HEIGHT);

				for (TextureAtlas* a : a_atlases)
					a->initialize(avgWidth, avgHeight, ATLAS_NUM_COMPONENTS, ATLAS_NUM_MIPS);
			}
			else
			{
				for (TextureAtlas* a : a_atlases)
					a->initialize(a->m_width * 2, a->m_width * 2, ATLAS_NUM_COMPONENTS, ATLAS_NUM_MIPS);
			}
		}
	}
}

bool containTexturesInAtlases(std::vector<TextureInfo>& a_textureInfoList, std::vector<TextureAtlas*>& a_atlases)
{
	if (!a_atlases.size())
		return false;
	for (TextureAtlas* a : a_atlases)
		a->clear();

	for (uint i = 0; i < a_textureInfoList.size(); ++i)
	{
		TextureInfo& tex = a_textureInfoList[i];
		bool contained = false;
		for (uint j = 0; j < a_atlases.size(); ++j)
		{
			TextureAtlas* atlas = a_atlases[j];
			TextureAtlas::AtlasRegion region = atlas->getRegion(tex.width, tex.height);
			if (region.width && region.height)
			{
				tex.atlasIdx = j;
				tex.region = region;
				contained = true;
				break;
			}
		}
		if (!contained)
			return false;
	}
	return true;
}

void fillAtlasTextures(const std::vector<TextureInfo>& a_textureAtlasRegions, const std::string& a_baseFilePath, std::vector<TextureAtlas*>& a_atlases)
{
	for (const TextureInfo& tex : a_textureAtlasRegions)
	{
		TextureAtlas* atlas = a_atlases[tex.atlasIdx];

		int width, height, numComponents;
		const std::string fullTexPath = getFolderPathForFile(a_baseFilePath) + tex.filePath;
		const unsigned char* data = stbi_load(fullTexPath.c_str(), &width, &height, &numComponents, atlas->m_numComponents);
		if (!data)
		{
			printf("FAILED TO LOAD IMAGE: %s \n", fullTexPath.c_str());
			assert(false);
		}
		assert(tex.width == tex.region.width);
		assert(tex.height == tex.region.height);
		//assert(atlas->m_numComponents == tex.numComp);
		atlas->setRegion(tex.region.x, tex.region.y, tex.region.width, tex.region.height, data, atlas->m_numComponents);
	}
}

uint getTextureInfoIndex(std::vector<TextureInfo>& a_textures, const std::string& a_baseFilePath, const std::string& a_texturePath)
{
	for (uint i = 0; i < a_textures.size(); ++i)
		if (a_textures[i].filePath == a_texturePath)
			return i;

	TextureInfo newTex;
	newTex.filePath = a_texturePath;
	const std::string fullPath = getFolderPathForFile(a_baseFilePath) + a_texturePath;
	newTex.result = stbi_info(fullPath.c_str(), &newTex.width, &newTex.height, &newTex.numComp);
	assert(newTex.result && "Failed to load texture");
	a_textures.push_back(newTex);
	return (uint) (a_textures.size() - 1);
}

void getMaterialTextureInfo(const aiScene& a_scene, const std::string& a_scenePath, 
							std::vector<MaterialData>& a_materials, std::vector<TextureInfo>& a_textures)
{
	const std::string baseTexturePath = getFolderPathForFile(a_scenePath);

	for (uint i = 0; i < a_scene.mNumMaterials; i++)
	{
		MaterialData materialData(i);
		const aiMaterial* material = a_scene.mMaterials[i];
		aiString path;

		auto textureInfoIndex = [&](aiTextureType a_texType)
		{
			if (material->GetTextureCount(a_texType) && material->GetTexture(a_texType, 0, &path) == AI_SUCCESS)
				return (int) getTextureInfoIndex(a_textures, baseTexturePath, path.C_Str());
			else
				return -1;
		};
		materialData.diffuseTextureInfoIndex = textureInfoIndex(aiTextureType_DIFFUSE);
		materialData.normalTextureInfoIndex = textureInfoIndex(getFileExtension(a_scenePath) == "obj" ? aiTextureType_HEIGHT : aiTextureType_NORMALS);

		a_materials.push_back(materialData);
	}
}

std::vector<MaterialProperty> getMaterialProperties(const std::vector<TextureAtlas*>& a_atlases, const std::vector<MaterialData>& a_materials, 
													const std::vector<TextureInfo>& a_textures)
{
	std::vector<MaterialProperty> materialProperties(a_materials.size());
	for (const MaterialData& mat : a_materials)
	{
		if (mat.diffuseTextureInfoIndex != -1)
		{
			const TextureInfo& tex = a_textures[mat.diffuseTextureInfoIndex];
			const TextureAtlas* atlas = a_atlases[tex.atlasIdx];
			materialProperties[mat.materialID].diffuseAtlasNr = tex.atlasIdx;
			materialProperties[mat.materialID].diffuseTexMapping = getTextureOffset(atlas->m_width, atlas->m_height, tex.region);
		}
		if (mat.normalTextureInfoIndex != -1)
		{
			const TextureInfo& tex = a_textures[mat.normalTextureInfoIndex];
			const TextureAtlas* atlas = a_atlases[tex.atlasIdx];
			materialProperties[mat.materialID].normalAtlasNr = tex.atlasIdx;
			materialProperties[mat.materialID].normalTexMapping = getTextureOffset(atlas->m_width, atlas->m_height, tex.region);
		}
	}
	if (!a_textures.size()) // If there are no textures, add set the dummy material
	{
		materialProperties[0].diffuseTexMapping = {0, 0, 0, 0};
		materialProperties[0].normalTexMapping = {0, 0, 0, 0};
		materialProperties[0].diffuseAtlasNr = 0;
		materialProperties[0].normalAtlasNr = 0;
	}
	return materialProperties; // c++11 so can return vector by value
}

void writeAtlasesToFiles(const std::vector<TextureAtlas*>& a_atlases, const std::string& a_outResourcePath)
{
	for (uint i = 0; i < (uint) a_atlases.size(); ++i)
	{
		const TextureAtlas* atlas = a_atlases[i];

		std::string atlasFileName(a_outResourcePath.substr(0, a_outResourcePath.find_last_of('.')));
		atlasFileName.append("-atlas-").append(std::to_string(i)).append(".da");

		std::ofstream file(atlasFileName.c_str(), std::ios::out | std::ios::binary);
		assert(file.is_open());

		const int type = (int) EResourceType::BYTEIMAGE;
		const int width = atlas->m_width;
		const int height = atlas->m_height;
		const int numComponents = atlas->m_numComponents;

		file.write(reinterpret_cast<const char*>(&type), sizeof(int));
		file.write(reinterpret_cast<const char*>(&width), sizeof(int));
		file.write(reinterpret_cast<const char*>(&height), sizeof(int));
		file.write(reinterpret_cast<const char*>(&numComponents), sizeof(int));
		file.write(reinterpret_cast<const char*>(atlas->m_data), width * height * numComponents);

		file.close();
	}
}

void getVerticesAndIndices(const aiScene& a_scene, std::vector<Vertex>& a_vertices, std::vector<uint>& a_indices)
{
	struct MeshEntry
	{
		uint meshIndex;
		uint baseVertex;
		uint materialIndex;
	};
	std::vector<MeshEntry> entries(a_scene.mNumMeshes);
	uint numVertices = 0;
	uint numIndices = 0;
	for (uint i = 0; i < a_scene.mNumMeshes; ++i)
	{
		entries[i].meshIndex = i;
		entries[i].materialIndex = a_scene.mMeshes[i]->mMaterialIndex;
		entries[i].baseVertex = numVertices;
		numVertices += a_scene.mMeshes[i]->mNumVertices;
		numIndices += a_scene.mMeshes[i]->mNumFaces * 3;
	}
	a_vertices.resize(numVertices);
	a_indices.resize(numIndices);

	uint indiceCounter = 0;
	uint vertexCounter = 0;
	for (uint i = 0; i < entries.size(); i++)
	{
		const MeshEntry& entry = entries[i];
		const aiMesh* mesh = a_scene.mMeshes[entry.meshIndex];
		const uint numVertices = mesh->mNumVertices;
		const uint numFaces = mesh->mNumFaces;
		bool hasTextureCoords = mesh->HasTextureCoords(0);
		bool hasTangentsAndBitangents = mesh->HasTangentsAndBitangents();

		for (uint j = 0; j < numFaces; ++j)
		{
			const aiFace& face = mesh->mFaces[j];
			a_indices[indiceCounter++] = face.mIndices[0] + entry.baseVertex;
			a_indices[indiceCounter++] = face.mIndices[1] + entry.baseVertex;
			a_indices[indiceCounter++] = face.mIndices[2] + entry.baseVertex;
		}

		for (uint j = 0; j < numVertices; ++j)
		{
			a_vertices[vertexCounter++] = Vertex(
					mesh->mVertices[j],
					hasTextureCoords ? mesh->mTextureCoords[0][j] : aiVector3D(0.0f),
					mesh->mNormals[j],
					hasTangentsAndBitangents ? mesh->mTangents[j] : aiVector3D(0.0f),
					hasTangentsAndBitangents ? mesh->mBitangents[j] : aiVector3D(0.0f),
					entry.materialIndex);
		}
	}

	assert(a_indices.size() == numIndices);
	assert(a_vertices.size() == numVertices);
}

END_UNNAMED_NAMESPACE()

bool ModelProcessor::process(const char* a_inResourcePath, const char* a_outResourcePath, std::vector<std::string>& a_rebuildDependencies)
{
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

	// First we get all the materials and textures used in the scene
	std::vector<MaterialData> materials;
	std::vector<TextureInfo> textures;
	getMaterialTextureInfo(*scene, inResourcePathStr, materials, textures);

	for (const TextureInfo& textureInfo : textures)
		a_rebuildDependencies.push_back(textureInfo.filePath);
	
	// Then we create atlasses out of the textures
	std::vector<TextureAtlas*> atlases;
	if (textures.size())
	{
		while (!containTexturesInAtlases(textures, atlases))
			increaseAtlasesSize(textures, atlases); // Allocates atlases
		fillAtlasTextures(textures, getFolderPathForFile(inResourcePathStr), atlases);
		writeAtlasesToFiles(atlases, outResourcePathstr);
	}
	const int numAtlases = (int) atlases.size();

	// Then we create the GLSL structs
	std::vector<MaterialProperty> matProperties = getMaterialProperties(atlases, materials, textures);
	for (TextureAtlas* a : atlases)
		delete a;
	atlases.clear();

	// Get vertices/indices and write everything to a file
	std::vector<Vertex> vertices;
	std::vector<uint> indices;
	getVerticesAndIndices(*scene, vertices, indices);

	aiReleaseImport(scene);
	scene = NULL;

	std::ofstream file(a_outResourcePath, std::ios::out | std::ios::binary);
	assert(file.is_open());

	const int type = (int) EResourceType::MODEL;
	file.write(reinterpret_cast<const char*>(&type), sizeof(int));
	file.write(reinterpret_cast<const char*>(&numAtlases), sizeof(int));
	writeVector(file, matProperties);
	writeVector(file, indices);
	writeVector(file, vertices);

	file.close();

	return true;
}