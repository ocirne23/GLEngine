#include "ModelProcessor.h"

#include <vector>
#include <fstream>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../Utils/stb_image.h"
#include "../Utils/TextureAtlas.h"

namespace
{
	enum { MAX_ATLAS_WIDTH = 2048, MAX_ATLAS_HEIGHT = 2048 };

	struct vec4 { float x, y, z, w; };
	struct vec3 { float x, y, z; };
	struct vec2 { float x, y; };

	struct MaterialProperty
	{
		MaterialProperty() : diffuseAtlasNr(-1), bumpAtlasNr(-1), specularAtlasNr(-1), maskAtlasNr(-1) {};
		vec4 diffuseTexMapping;
		vec4 bumpTexMapping;
		vec4 specTexMapping;
		vec4 maskTexMapping;
		int diffuseAtlasNr;
		int bumpAtlasNr;
		int specularAtlasNr;
		int maskAtlasNr;
	};
	struct MaterialFiles
	{
		std::string diffuse;
		std::string bump;
		std::string spec;
		std::string mask;
	};
	struct AtlasRegion
	{
		std::string image;
		int atlasNr;
		TextureAtlas* atlas;
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
		Vertex(aiVector3D& position, aiVector3D& texcoords, aiVector3D& normal, aiVector3D& tangents, aiVector3D& bitangents, unsigned int materialID)
			: position((vec3&) position), texcoords((vec2&) texcoords), normal((vec3&) normal)
			, tangents((vec3&) tangents), bitangents((vec3&) bitangents), materialID(materialID) {}
		vec3 position;
		vec2 texcoords;
		vec3 normal;
		vec3 tangents;
		vec3 bitangents;
		unsigned int materialID;
	};

	template <typename T>
	void writeVector(std::ostream& file, const std::vector<T>& vector)
	{
		int size = (int) vector.size();
		file.write(reinterpret_cast<const char*>(&size), sizeof(int));
		file.write(reinterpret_cast<const char*>(&vector[0]), sizeof(vector[0]) * size);
	}
	vec4 getTextureOffset(const AtlasRegion& reg)
	{
		int atlasWidth = (int) reg.atlas->m_width;
		int atlasHeight = (int) reg.atlas->m_height;

		int regionWidth = reg.region.width;
		int regionHeight = reg.region.height;

		int regionX = reg.region.x;
		int regionY = reg.region.y;

		float wScale = regionWidth / (float) atlasWidth;
		float hScale = regionHeight / (float) atlasHeight;

		float xOffset = regionX / (float) atlasWidth;
		float yOffset = regionY / (float) atlasHeight;

		return { xOffset, yOffset, wScale, hScale };
	}

	bool writeRaw(const std::string& srcFilePath, const std::string& dstFilePath, bool flipUV)
	{
		unsigned int flags = 0
			| aiPostProcessSteps::aiProcess_Triangulate
			| aiPostProcessSteps::aiProcess_CalcTangentSpace
			| aiPostProcessSteps::aiProcess_GenNormals;
		if (flipUV)
			flags |= aiPostProcessSteps::aiProcess_FlipUVs;
		const aiScene* scene = aiImportFile(srcFilePath.c_str(), flags);

		if (!scene)
		{
			printf("Error parsing scene '%s' : %s\n", srcFilePath.c_str(), aiGetErrorString());
			return false;
		}

		std::string texturePathStr(srcFilePath);
		std::string::size_type idx = texturePathStr.find_last_of('/');
		if (idx == std::string::npos)
			idx = texturePathStr.find_last_of('\\');
		texturePathStr = texturePathStr.substr(0, idx).append("\\");
		
		std::vector<MaterialFiles> matFiles(scene->mNumMaterials);
		std::vector<std::string> textures;

		printf("Num materials in scene: %i \n", scene->mNumMaterials);
		for (unsigned int i = 0; i < scene->mNumMaterials; i++)
		{
			aiMaterial* material = scene->mMaterials[i];
			/*
			assert(i <= MAX_MATERIALS && "Cannot have more than MAX_MATERIALS materials in mesh");
			if (i > MAX_MATERIALS)
				break;
			*/
			MaterialFiles& matProperty = matFiles[i];

			int numDiffuse = material->GetTextureCount(aiTextureType_DIFFUSE);
			int numBump = material->GetTextureCount(aiTextureType_HEIGHT);
			int numSpecular = material->GetTextureCount(aiTextureType_SPECULAR);
			int numMask = material->GetTextureCount(aiTextureType_OPACITY);

			//assert(numDiffuse || numBump || numSpecular || numMask);

			aiString path;
			if (numDiffuse > 0 && material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
			{
				 matProperty.diffuse = texturePathStr;
				 matProperty.diffuse.append(path.C_Str());
				 if (std::find(textures.begin(), textures.end(), matProperty.diffuse) == textures.end())
					 textures.push_back(matProperty.diffuse);
			}
			if (numBump > 0 && material->GetTexture(aiTextureType_HEIGHT, 0, &path) == AI_SUCCESS)
			{
				matProperty.bump = texturePathStr;
				matProperty.bump.append(path.C_Str());
				if (std::find(textures.begin(), textures.end(), matProperty.bump) == textures.end())
					textures.push_back(matProperty.bump);
			}
			if (numSpecular > 0 && material->GetTexture(aiTextureType_SPECULAR, 0, &path) == AI_SUCCESS)
			{
				matProperty.spec = texturePathStr;
				matProperty.spec.append(path.C_Str());				
				if (std::find(textures.begin(), textures.end(), matProperty.spec) == textures.end())
					textures.push_back(matProperty.spec);
			}
			if (numMask > 0 && material->GetTexture(aiTextureType_OPACITY, 0, &path) == AI_SUCCESS)
			{
				matProperty.mask = texturePathStr;
				matProperty.mask.append(path.C_Str());				
				if (std::find(textures.begin(), textures.end(), matProperty.mask) == textures.end())
					textures.push_back(matProperty.mask);
			}
		}

		std::vector<MeshEntry> entries;
		std::vector<MeshEntry> transparentEntries;

		std::vector<AtlasRegion> atlasRegions;
		std::vector<TextureAtlas*> atlasses;

		for (const std::string& str : textures)
		{
			int width, height, numComp;
			int result = stbi_info(str.c_str(), &width, &height, &numComp);
			if (!result)
			{
				printf("Cannot open file: %s \n", str.c_str());
				continue;
			}

			if (!(numComp == 1 || numComp == 3))
			{
				printf("Invalid number of components: %i in texture: %s \n", numComp, str.c_str());
			}

			bool contained = false;
			for (int i = 0; i < atlasses.size(); ++i)
			{
				TextureAtlas* atlas = atlasses[i];

				if (atlas->m_numComponents == numComp)
				{
					TextureAtlas::AtlasRegion region = atlas->getRegion(width, height);
					if (region.width && region.height)
					{
						contained = true;
						atlasRegions.push_back({ str, i, atlas, region });
						break;
					}
				}
			}
			if (!contained)
			{
				TextureAtlas* atlas = new TextureAtlas(MAX_ATLAS_WIDTH, MAX_ATLAS_HEIGHT, numComp);
				TextureAtlas::AtlasRegion region = atlas->getRegion(width, height);
				assert(region.width && region.height);
				atlasses.push_back(atlas);
				atlasRegions.push_back({ str, (int) atlasses.size() - 1, atlas, region });
			}
		}

		std::vector<MaterialProperty> matProperties(matFiles.size());

		for (int i = 0; i < matFiles.size(); ++i)
		{
			MaterialFiles& files = matFiles[i];
			MaterialProperty& prop = matProperties[i];

			for (const AtlasRegion& reg : atlasRegions)
			{
				if (files.diffuse == reg.image)
				{
					prop.diffuseTexMapping = getTextureOffset(reg);
					prop.diffuseAtlasNr = reg.atlasNr;
				}
				else if (files.bump == reg.image)
				{
					prop.bumpTexMapping = getTextureOffset(reg);
					prop.bumpAtlasNr = reg.atlasNr;
				}
				else if (files.spec == reg.image)
				{
					prop.specTexMapping = getTextureOffset(reg);
					prop.specularAtlasNr = reg.atlasNr;
				}
				else if (files.mask == reg.image)
				{
					prop.maskTexMapping = getTextureOffset(reg);
					prop.maskAtlasNr = reg.atlasNr;
				}
				else
				{
				//	assert(false); // no textures in material?
				}
			}
			
			printf("dif %i %f %f %f %f\n", prop.diffuseAtlasNr, prop.diffuseTexMapping.x, prop.diffuseTexMapping.y, prop.diffuseTexMapping.z, prop.diffuseTexMapping.w);
			printf("bump %i %f %f %f %f\n", prop.bumpAtlasNr, prop.bumpTexMapping.x, prop.bumpTexMapping.y, prop.bumpTexMapping.z, prop.bumpTexMapping.w);
			printf("spec %i %f %f %f %f\n", prop.specularAtlasNr, prop.specTexMapping.x, prop.specTexMapping.y, prop.specTexMapping.z, prop.specTexMapping.w);
			printf("mask %i %f %f %f %f\n\n", prop.maskAtlasNr, prop.maskTexMapping.x, prop.maskTexMapping.y, prop.maskTexMapping.z, prop.maskTexMapping.w);
		}

		for (const AtlasRegion& region : atlasRegions)
		{
			int width, height, numComponents;
			const unsigned char* data = stbi_load(region.image.c_str(), &width, &height, &numComponents, 0);
			if (!data)
			{
				printf("FAILED TO LOAD IMAGE: %s \n", region.image.c_str());
				continue;
			}

			region.atlas->setRegion(region.region.x, region.region.y, region.region.width, region.region.height, data, numComponents);
		}

		std::string dstTexturePathStr(dstFilePath);
		dstTexturePathStr = dstTexturePathStr.substr(0, dstTexturePathStr.find_last_of('.'));

		std::sort(atlasses.begin(), atlasses.end(), [](const TextureAtlas* a, const TextureAtlas* b)
		{
			return a->m_numComponents < b->m_numComponents;
		});

		int num1CompAtlasses = 0, num3CompAtlasses = 0, num4CompAtlasses = 0;
		for (const TextureAtlas* atlas : atlasses)
		{
			switch (atlas->m_numComponents)
			{
			case 1: num1CompAtlasses++; break;
			case 3: num3CompAtlasses++; break;
			case 4: num4CompAtlasses++; break;
			default: assert(false); break;
			}
		}

		for (int i = 0; i < atlasses.size(); ++i)
		{
			TextureAtlas* atlas = atlasses[i];
			
			std::string atlasFileName = dstTexturePathStr.c_str();
			atlasFileName.append("-atlas-").append(std::to_string(i)).append(".da");

			std::ofstream file(atlasFileName.c_str(), std::ios::out | std::ios::binary);
			assert(file.is_open());

			int type, width, height, numComponents;
			type = ResourceType_BYTEIMAGE;
			width = (int) atlas->m_width;
			height = (int) atlas->m_height;
			numComponents = (int) atlas->m_numComponents;

			file.write(reinterpret_cast<const char*>(&type), sizeof(int));
			file.write(reinterpret_cast<const char*>(&width), sizeof(int));
			file.write(reinterpret_cast<const char*>(&height), sizeof(int));
			file.write(reinterpret_cast<const char*>(&numComponents), sizeof(int));
			file.write(reinterpret_cast<const char*>(atlas->m_data), width * height * numComponents);

			file.close();
		}

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
			if (matProperties[entry.materialIndex].maskAtlasNr != -1)
			{
				entry.baseVertex = numTransparentVerticesInScene;
				entry.baseIndex = numTransparentIndicesInScene;
				numTransparentVerticesInScene += scene->mMeshes[i]->mNumVertices;
				numTransparentIndicesInScene += entry.numIndices;
				transparentEntries.push_back(entry);
			}
			else
			{
				entry.baseVertex = numVerticesInScene;
				entry.baseIndex = numIndicesInScene;
				numVerticesInScene += scene->mMeshes[i]->mNumVertices;
				numIndicesInScene += entry.numIndices;
				entries.push_back(entry);
			}
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

		std::ofstream file(dstFilePath.c_str(), std::ios::out | std::ios::binary);
		assert(file.is_open());

		int type = ResourceType_MODEL;
		file.write(reinterpret_cast<const char*>(&type), sizeof(int));
		file.write(reinterpret_cast<const char*>(&num1CompAtlasses), sizeof(int));
		file.write(reinterpret_cast<const char*>(&num3CompAtlasses), sizeof(int));
		int numOpague = (int) transparentEntries.size();
		file.write(reinterpret_cast<const char*>(&numOpague), sizeof(int));
		writeVector(file, indiceCounts);
		writeVector(file, baseIndices);
		writeVector(file, matProperties);
		writeVector(file, indices);
		writeVector(file, vertices);

		file.close();

		return true;
	}
}

bool ModelProcessor::process(const char* inResourcePath, const char* outResourcePath)
{
	return writeRaw(inResourcePath, outResourcePath, true);
}