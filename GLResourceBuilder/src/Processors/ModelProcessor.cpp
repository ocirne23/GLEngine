#include "ModelProcessor.h"

#include "EResourceType.h"
#include "Utils/stb_image.h"
#include "Utils/TextureAtlas.h"

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <fstream>

namespace
{
	enum { MAX_ATLAS_WIDTH = 8192, MAX_ATLAS_HEIGHT = 8192, NUM_ATLAS_MIPS = 4 };
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

	vec4 getTextureOffset(const AtlasRegion& a_reg)
	{
		const int atlasWidth = (int) a_reg.atlas->m_width;
		const int atlasHeight = (int)a_reg.atlas->m_height;

		const int regionWidth = a_reg.region.width;
		const int regionHeight = a_reg.region.height;

		const int regionX = a_reg.region.x;
		const int regionY = a_reg.region.y;

		const float wScale = regionWidth / (float) atlasWidth;
		const float hScale = regionHeight / (float) atlasHeight;

		const float xOffset = regionX / (float) atlasWidth;
		const float yOffset = regionY / (float) atlasHeight;

		return { xOffset, yOffset, wScale, hScale };
	}
}

bool ModelProcessor::process(const char* a_inResourcePath, const char* a_outResourcePath)
{
	unsigned int flags = 0
		| aiPostProcessSteps::aiProcess_Triangulate
		| aiPostProcessSteps::aiProcess_CalcTangentSpace
		| aiPostProcessSteps::aiProcess_GenNormals;
	// Flip uv's because OpenGL
	flags |= aiPostProcessSteps::aiProcess_FlipUVs;

	const aiScene* scene = aiImportFile(a_inResourcePath, flags);

	if (!scene)
	{
		printf("Error parsing scene '%s' : %s\n", a_inResourcePath, aiGetErrorString());
		return false;
	}

	std::string texturePathStr(a_inResourcePath);
	std::string::size_type idx = texturePathStr.find_last_of('/');
	if (idx == std::string::npos)
		idx = texturePathStr.find_last_of('\\');
	texturePathStr = texturePathStr.substr(0, idx).append("\\");

	std::vector<MaterialFiles> matFiles(scene->mNumMaterials);
	struct TextureTypePath
	{
		enum ETextureType
		{
			ETextureType_DIFFUSE, ETextureType_SPECULAR, ETextureType_BUMP, ETextureType_MASK
		};
		std::string path;
		ETextureType type;
	};
	std::vector<TextureTypePath> textures;

	for (unsigned int i = 0; i < scene->mNumMaterials; i++)
	{
		const aiMaterial* material = scene->mMaterials[i];
		MaterialFiles& matProperty = matFiles[i];

		int numDiffuse = material->GetTextureCount(aiTextureType_DIFFUSE);
		int numBump = material->GetTextureCount(aiTextureType_HEIGHT);
		int numSpecular = material->GetTextureCount(aiTextureType_SPECULAR);
		int numMask = material->GetTextureCount(aiTextureType_OPACITY);

		// assert(numDiffuse || numBump || numSpecular || numMask); // Check that there are textures?

		aiString path;
		if (numDiffuse > 0 && material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
		{
			matProperty.diffuse = texturePathStr;
			matProperty.diffuse.append(path.C_Str());
			if (std::find_if(textures.begin(), textures.end(), [&](const TextureTypePath& t) { return t.path == matProperty.diffuse; }) == textures.end())
				textures.push_back({ matProperty.diffuse, TextureTypePath::ETextureType_DIFFUSE });
		}
		if (numBump > 0 && material->GetTexture(aiTextureType_HEIGHT, 0, &path) == AI_SUCCESS)
		{
			matProperty.bump = texturePathStr;
			matProperty.bump.append(path.C_Str());
			if (std::find_if(textures.begin(), textures.end(), [&](const TextureTypePath& t) { return t.path == matProperty.bump; }) == textures.end())
				textures.push_back({ matProperty.bump, TextureTypePath::ETextureType_BUMP });
		}
		if (numSpecular > 0 && material->GetTexture(aiTextureType_SPECULAR, 0, &path) == AI_SUCCESS)
		{
			matProperty.spec = texturePathStr;
			matProperty.spec.append(path.C_Str());
			if (std::find_if(textures.begin(), textures.end(), [&](const TextureTypePath& t) { return t.path == matProperty.spec; }) == textures.end())
				textures.push_back({ matProperty.spec, TextureTypePath::ETextureType_SPECULAR });
		}
		if (numMask > 0 && material->GetTexture(aiTextureType_OPACITY, 0, &path) == AI_SUCCESS)
		{
			matProperty.mask = texturePathStr;
			matProperty.mask.append(path.C_Str());
			if (std::find_if(textures.begin(), textures.end(), [&](const TextureTypePath& t) { return t.path == matProperty.mask; }) == textures.end())
				textures.push_back({ matProperty.mask, TextureTypePath::ETextureType_MASK });
		}
	}

	std::vector<MeshEntry> entries;
	std::vector<MeshEntry> transparentEntries;
	std::vector<AtlasRegion> atlasRegions;
	std::vector<TextureAtlas*> atlasses;

	for (const TextureTypePath& tex : textures)
	{
		int width, height, numComp;
		int result = stbi_info(tex.path.c_str(), &width, &height, &numComp);

		if (!result)
		{
			printf("Cannot open file: %s \n", tex.path.c_str());
			continue;
		}

		switch (tex.type)
		{
		case TextureTypePath::ETextureType_DIFFUSE:  numComp = 3; break;
		case TextureTypePath::ETextureType_BUMP:     numComp = 3; break;
		case TextureTypePath::ETextureType_SPECULAR: numComp = 1; break;
		case TextureTypePath::ETextureType_MASK:     numComp = 1; break;
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
					atlasRegions.push_back({ tex.path, i, atlas, region });
					break;
				}
			}
		}
		if (!contained)
		{
			TextureAtlas* atlas = new TextureAtlas(MAX_ATLAS_WIDTH, MAX_ATLAS_HEIGHT, numComp, NUM_ATLAS_MIPS);
			TextureAtlas::AtlasRegion region = atlas->getRegion(width, height);
			assert(region.width && region.height);
			atlasses.push_back(atlas);
			atlasRegions.push_back({ tex.path, (int)atlasses.size() - 1, atlas, region });
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
	}

	for (const AtlasRegion& region : atlasRegions)
	{
		int width, height, numComponents;
		const unsigned char* data = stbi_load(region.image.c_str(), &width, &height, &numComponents, region.atlas->m_numComponents);
		if (!data)
		{
			printf("FAILED TO LOAD IMAGE: %s \n", region.image.c_str());
			continue;
		}

		region.atlas->setRegion(region.region.x, region.region.y, region.region.width, region.region.height, data, region.atlas->m_numComponents);
	}

	std::string dstTexturePathStr(a_outResourcePath);
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
		type = EResourceType_BYTEIMAGE;
		width = (int)atlas->m_width;
		height = (int)atlas->m_height;
		numComponents = (int)atlas->m_numComponents;

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

	std::ofstream file(a_outResourcePath, std::ios::out | std::ios::binary);
	assert(file.is_open());

	const int type = EResourceType_MODEL;
	file.write(reinterpret_cast<const char*>(&type), sizeof(int));
	file.write(reinterpret_cast<const char*>(&num1CompAtlasses), sizeof(int));
	file.write(reinterpret_cast<const char*>(&num3CompAtlasses), sizeof(int));
	const int numOpague = (int)transparentEntries.size();
	file.write(reinterpret_cast<const char*>(&numOpague), sizeof(int));
	writeVector(file, indiceCounts);
	writeVector(file, baseIndices);
	writeVector(file, matProperties);
	writeVector(file, indices);
	writeVector(file, vertices);

	file.close();

	return true;
}