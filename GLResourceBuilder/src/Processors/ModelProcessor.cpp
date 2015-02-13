#include "ModelProcessor.h"

#include "EResourceType.h"
#include "Utils/stb_image.h"
#include "Utils/TextureAtlas.h"

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <fstream>

#include <iostream>

namespace
{
	enum { ATLAS_MAX_WIDTH = 8192, ATLAS_MAX_HEIGHT = 8192, ATLAS_NUM_MIPS = 4, ATLAS_NUM_COMPONENTS = 3, ATLAS_START_WIDTH = 256, ATLAS_START_HEIGHT = 256, ATLAS_INCREMENT = 256 };
	struct vec4 { float x, y, z, w; };
	struct vec3 { float x, y, z; };
	struct vec2 { float x, y; };
	struct MaterialProperty
	{
		vec4 diffuseTexMapping;
		vec4 normalTexMapping;
		int diffuseAtlasNr	= -1;
		int normalAtlasNr	= -1;
		int padding			= -1;
		int padding2		= -1;
	};
	struct TextureInfo
	{
		std::string filePath;
		int result, width, height, numComp;
	};
	struct TextureAtlasRegion
	{
		aiTextureType type;
		unsigned int materialID;
		
		unsigned int atlasIdx;
		TextureInfo texture;
		TextureAtlas::AtlasRegion region;
	};
	struct MeshEntry
	{
		unsigned int meshIndex;
		unsigned int numIndices;
		unsigned int baseVertex;
		unsigned int baseIndex;
		unsigned int materialIndex;
	};
	struct Vertex
	{
		Vertex() {};
		Vertex(aiVector3D& a_position, aiVector3D& a_texcoords, aiVector3D& a_normal, aiVector3D& a_tangents, aiVector3D& a_bitangents, unsigned int a_materialID)
			: position((vec3&) a_position), texcoords((vec2&) a_texcoords), normal((vec3&) a_normal)
			, tangents((vec3&) a_tangents), bitangents((vec3&)a_bitangents), materialID(a_materialID) {}
		vec3 position;
		vec2 texcoords;
		vec3 normal;
		vec3 tangents;
		vec3 bitangents;
		unsigned int materialID;
	};

	template <typename T>
	void writeVector(std::ostream& a_file, const std::vector<T>& a_vector)
	{
		const int size = (int) a_vector.size();
		a_file.write(reinterpret_cast<const char*>(&size), sizeof(int));
		a_file.write(reinterpret_cast<const char*>(&a_vector[0]), sizeof(a_vector[0]) * size);
	}

	vec4 getTextureOffset(unsigned int a_atlasWidth, unsigned int a_atlasHeight, const TextureAtlas::AtlasRegion& a_reg)
	{
		const int regionWidth = a_reg.width;
		const int regionHeight = a_reg.height;

		const int regionX = a_reg.x;
		const int regionY = a_reg.y;

		const float wScale = regionWidth / (float) a_atlasWidth;
		const float hScale = regionHeight / (float) a_atlasHeight;

		const float xOffset = regionX / (float) a_atlasWidth;
		const float yOffset = regionY / (float) a_atlasHeight;

		return { xOffset, yOffset, wScale, hScale };
	}

	TextureInfo getTextureInfo(const std::string& a_path)
	{
		TextureInfo info;
		info.filePath = a_path;
		info.result = stbi_info(a_path.c_str(), &info.width, &info.height, &info.numComp);
		return info;
	}

	void increaseAtlasesSize(std::vector<TextureAtlas*>& a_atlases)
	{
		if (!a_atlases.size())
		{
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

				if (avgHeight % 2 != 0)
					avgHeight++;
				if (avgWidth % 2 != 0)
					avgWidth++;

				avgWidth = std::min(avgWidth, (int) ATLAS_MAX_WIDTH);
				avgHeight = std::min(avgHeight, (int) ATLAS_MAX_HEIGHT);

				for (TextureAtlas* a : a_atlases)
					a->initialize(avgWidth, avgHeight, ATLAS_NUM_COMPONENTS, ATLAS_NUM_MIPS);
			}
		}
	}

	bool containTexturesInAtlases(std::vector<TextureAtlasRegion>& a_textureAtlasRegions, std::vector<TextureAtlas*>& a_atlases)
	{
		if (!a_atlases.size())
			return false;

		for (TextureAtlas* a : a_atlases)
			a->clear();

		for (unsigned int i = 0; i < a_textureAtlasRegions.size(); ++i)
		{
			bool alreadyAdded = false;
			TextureAtlasRegion& tar = a_textureAtlasRegions[i];
			for (unsigned int j = 0; j < i; ++j)
			{
				TextureAtlasRegion& prev = a_textureAtlasRegions[j];
				if (prev.texture.filePath == tar.texture.filePath)
				{
					alreadyAdded = true;
					tar.atlasIdx = prev.atlasIdx;
					tar.region = prev.region;
					break;
				}
			}

			if (!alreadyAdded)
			{
				bool contained = false;
				for (unsigned int j = 0; j < a_atlases.size(); ++j)
				{
					TextureAtlas* atlas = a_atlases[j];

					TextureAtlas::AtlasRegion region = atlas->getRegion(tar.texture.width, tar.texture.height);
					if (region.width && region.height)
					{
						tar.atlasIdx = j;
						tar.region = region;
						contained = true;
						break;
					}
				}
				if (!contained)
					return false;
			}
		}
		return true;
	}

	void fillAtlasTextures(const std::vector<TextureAtlasRegion>& a_textureAtlasRegions, std::vector<TextureAtlas*>& a_atlases)
	{
		for (const TextureAtlasRegion& tar : a_textureAtlasRegions)
		{
			TextureAtlas* atlas = a_atlases[tar.atlasIdx];

			int width, height, numComponents;
			const unsigned char* data = stbi_load(tar.texture.filePath.c_str(), &width, &height, &numComponents, atlas->m_numComponents);
			if (!data)
			{
				printf("FAILED TO LOAD IMAGE: %s \n", tar.texture.filePath.c_str());
				continue;
			}
			atlas->setRegion(tar.region.x, tar.region.y, tar.region.width, tar.region.height, data, atlas->m_numComponents);
		}
	}
}

bool ModelProcessor::process(const char* a_inResourcePath, const char* a_outResourcePath)
{
	const std::string inResourcePathStr(a_inResourcePath);
	const std::string outResourcePathstr(a_outResourcePath);

	const unsigned int flags = 0
		| aiPostProcessSteps::aiProcess_Triangulate
		| aiPostProcessSteps::aiProcess_CalcTangentSpace
		| aiPostProcessSteps::aiProcess_GenNormals
		| aiPostProcessSteps::aiProcess_FlipUVs; // Flip uv's because OpenGL

	const aiScene* scene = aiImportFile(a_inResourcePath, flags);

	if (!scene)
	{
		printf("Error parsing scene '%s' : %s\n", a_inResourcePath, aiGetErrorString());
		return false;
	}

	const std::string extension = inResourcePathStr.substr(inResourcePathStr.find_last_of('.') + 1, inResourcePathStr.length() - 1);
	const std::string baseTexturePath = inResourcePathStr.substr(0, std::min(inResourcePathStr.find_last_of('/'), inResourcePathStr.find_last_of('\\'))).append("\\");

	std::vector<TextureAtlasRegion> textureAtlasRegions;

	for (unsigned int i = 0; i < scene->mNumMaterials; i++)
	{
		const aiMaterial* material = scene->mMaterials[i];
		
		const int numDiffuse = material->GetTextureCount(aiTextureType_DIFFUSE);
		const int numNormal = material->GetTextureCount(aiTextureType_HEIGHT);
		const int numSpecular = material->GetTextureCount(aiTextureType_SPECULAR);
		const int numMask = material->GetTextureCount(aiTextureType_OPACITY);
		assert(numDiffuse <= 1 && numNormal <= 1 && numSpecular <= 1 && numMask <= 1 && "More than one texture of the same type in material");
		//assert((numDiffuse || numNormal || numSpecular || numMask) && "No textures in material"); // Check that there are textures?

		auto addTextureAtlasRegion = [&](const char* a_imageName, int a_materialID, aiTextureType a_type) {
			std::string filePath(baseTexturePath);
			filePath.append(a_imageName);

			TextureAtlasRegion tar;
			tar.materialID = a_materialID;
			tar.type = a_type;
			tar.texture = getTextureInfo(filePath);
			textureAtlasRegions.push_back(tar);
		};

		aiString path;
		if (numDiffuse > 0 && material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
			addTextureAtlasRegion(path.C_Str(), i, aiTextureType_DIFFUSE);
		if (numNormal > 0 && material->GetTexture(extension == "obj" ? aiTextureType_HEIGHT : aiTextureType_NORMALS, 0, &path) == AI_SUCCESS)
			addTextureAtlasRegion(path.C_Str(), i, aiTextureType_NORMALS);
		
		/* TODO: specular and mask maps
		if (numSpecular > 0 && material->GetTexture(aiTextureType_SPECULAR, 0, &path) == AI_SUCCESS)
			addTextureAtlasRegion(path.C_Str(), i);
		if (numMask > 0 && material->GetTexture(aiTextureType_OPACITY, 0, &path) == AI_SUCCESS)
			addTextureAtlasRegion(path.C_Str(), i);
		*/
	}

	std::vector<TextureAtlas*> atlases;
	while (!containTexturesInAtlases(textureAtlasRegions, atlases))
		increaseAtlasesSize(atlases);
	fillAtlasTextures(textureAtlasRegions, atlases);

	std::vector<MaterialProperty> matProperties(scene->mNumMaterials);
	for (const TextureAtlasRegion& tar : textureAtlasRegions)
	{
		const TextureAtlas* atlas = atlases[tar.atlasIdx];
		switch (tar.type)
		{
		case aiTextureType_DIFFUSE:
		{
			matProperties[tar.materialID].diffuseAtlasNr = tar.atlasIdx;
			matProperties[tar.materialID].diffuseTexMapping = getTextureOffset(atlas->m_width, atlas->m_height, tar.region);
			break;
		}
		case aiTextureType_NORMALS:
		{
			matProperties[tar.materialID].normalAtlasNr = tar.atlasIdx;
			matProperties[tar.materialID].normalTexMapping = getTextureOffset(atlas->m_width, atlas->m_height, tar.region);
			break;
		}
		}
	}

	for (int i = 0; i < atlases.size(); ++i)
	{
		const TextureAtlas* atlas = atlases[i];

		std::string atlasFileName(outResourcePathstr.substr(0, outResourcePathstr.find_last_of('.')));
		atlasFileName.append("-atlas-").append(std::to_string(i)).append(".da");

		std::ofstream file(atlasFileName.c_str(), std::ios::out | std::ios::binary);
		assert(file.is_open());

		const int type = EResourceType_BYTEIMAGE;
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

	const int numAtlases = (int) atlases.size();

	for (TextureAtlas* a : atlases)
		delete a;
	atlases.clear();

	std::vector<MeshEntry> transparentEntries;
	std::vector<MeshEntry> entries;
	entries.reserve(scene->mNumMeshes);

	unsigned int numVerticesInScene = 0;
	unsigned int numIndicesInScene = 0;

	unsigned int numTransparentVerticesInScene = 0;
	unsigned int numTransparentIndicesInScene = 0;

	for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
	{
		MeshEntry entry;
		entry.meshIndex = i;
		entry.materialIndex = scene->mMeshes[i]->mMaterialIndex;
		entry.numIndices = scene->mMeshes[i]->mNumFaces * 3;
		/*
		if (matProperties[entry.materialIndex].maskAtlasNr != -1)
		{
			entry.baseVertex = numTransparentVerticesInScene;
			entry.baseIndex = numTransparentIndicesInScene;
			numTransparentVerticesInScene += scene->mMeshes[i]->mNumVertices;
			numTransparentIndicesInScene += entry.numIndices;
			transparentEntries.push_back(entry);
		}
		else
		{ */
			entry.baseVertex = numVerticesInScene;
			entry.baseIndex = numIndicesInScene;
			numVerticesInScene += scene->mMeshes[i]->mNumVertices;
			numIndicesInScene += entry.numIndices;
			entries.push_back(entry);
		//}
	}

	for (MeshEntry& entry : transparentEntries)
	{
		entry.baseVertex += numVerticesInScene;
		entry.baseIndex += numIndicesInScene;
		entries.push_back(entry);
	}

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	vertices.reserve(numVerticesInScene);
	indices.reserve(numIndicesInScene);

	std::vector<unsigned int> indiceCounts;
	std::vector<unsigned int> baseIndices;
	indiceCounts.reserve(entries.size());
	baseIndices.reserve(entries.size());

	for (unsigned int i = 0; i < entries.size(); i++)
	{
		const MeshEntry& entry = entries[i];
		indiceCounts.push_back(entry.numIndices);
		baseIndices.push_back(entry.baseIndex * sizeof(unsigned int));
	}

	for (unsigned int i = 0; i < entries.size(); i++)
	{
		const MeshEntry& entry = entries[i];
		const aiMesh* mesh = scene->mMeshes[entry.meshIndex];
		const unsigned int numVertices = mesh->mNumVertices;
		const unsigned int numFaces = mesh->mNumFaces;

		for (unsigned int j = 0; j < numFaces; ++j)
		{
			const aiFace& face = mesh->mFaces[j];
			indices.push_back(face.mIndices[0] + entry.baseVertex);
			indices.push_back(face.mIndices[1] + entry.baseVertex);
			indices.push_back(face.mIndices[2] + entry.baseVertex);
		}

		for (unsigned int j = 0; j < numVertices; ++j)
		{
			vertices.emplace_back(
				mesh->mVertices[j],
				mesh->HasTextureCoords(0) ? mesh->mTextureCoords[0][j] : aiVector3D(0.0f),
				mesh->mNormals[j],
				mesh->mTangents[j],
				mesh->mBitangents[j],
				entry.materialIndex);
		}
	}
	aiReleaseImport(scene);

	std::ofstream file(a_outResourcePath, std::ios::out | std::ios::binary);
	assert(file.is_open());

	const int type = EResourceType_MODEL;
	const int numTransparent = (int) transparentEntries.size();

	file.write(reinterpret_cast<const char*>(&type), sizeof(int));
	file.write(reinterpret_cast<const char*>(&numAtlases), sizeof(int));
	file.write(reinterpret_cast<const char*>(&numTransparent), sizeof(int));
	//writeVector(file, indiceCounts);
	//writeVector(file, baseIndices);
	writeVector(file, matProperties);
	writeVector(file, indices);
	writeVector(file, vertices);

	file.close();

	return true;
}