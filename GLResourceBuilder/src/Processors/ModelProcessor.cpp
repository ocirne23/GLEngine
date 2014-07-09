#include "ModelProcessor.h"

#include <vector>
#include <fstream>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace
{
enum { MAX_MATERIALS = 200 };

struct vec4 { float x, y, z, w; };
struct vec3 { float x, y, z; };
struct vec2 { float x, y; };

struct IOMaterialProperty
{
	IOMaterialProperty()
	{
		memset(this, 0, sizeof(IOMaterialProperty));
	};
	vec4 diffuseColorAndAlpha;
	vec4 specularColorAndExp;
	vec4 materialEmissiveAndPower;
	char diffuseTex[_MAX_PATH];
	char bumpTex[_MAX_PATH];
	char specularTex[_MAX_PATH];
	char maskTex[_MAX_PATH];
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
	printf("Size: %i \n", size);
	
	file.write(reinterpret_cast<const char*>(&size), sizeof(int));
	file.write(reinterpret_cast<const char*>(&vector[0]), sizeof(vector[0]) * size);
}

void writeRaw(const std::string& srcFilePath, const std::string& dstFilePath, bool flipUV)
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
		return;
	}

	std::vector<IOMaterialProperty> matProperties;
	matProperties.resize(scene->mNumMaterials);
	for (unsigned int i = 0; i < scene->mNumMaterials; i++)
	{
		aiMaterial* material = scene->mMaterials[i];

		assert(i <= MAX_MATERIALS && "Cannot have more than MAX_MATERIALS materials in mesh");
		if (i > MAX_MATERIALS)
			break;
		IOMaterialProperty& matProperty = matProperties[i];

		int numDiffuse = material->GetTextureCount(aiTextureType_DIFFUSE);
		int numBump = material->GetTextureCount(aiTextureType_HEIGHT);
		int numSpecular = material->GetTextureCount(aiTextureType_SPECULAR);
		int numMask = material->GetTextureCount(aiTextureType_OPACITY);

		aiColor3D difCol(1.0f, 1.0f, 1.0f);
		material->Get(AI_MATKEY_COLOR_DIFFUSE, difCol);
		float alpha = 1.0f;
		material->Get(AI_MATKEY_OPACITY, alpha);
		matProperty.diffuseColorAndAlpha = {difCol.r, difCol.g, difCol.b, alpha};

		aiColor3D specCol(1.0f, 1.0f, 1.0f);
		material->Get(AI_MATKEY_COLOR_SPECULAR, specCol);
		float specPow = 0.0f;
		material->Get(AI_MATKEY_SHININESS_STRENGTH, specPow);
		matProperty.specularColorAndExp = { specCol.r, specCol.g, specCol.b, specPow };

		aiColor3D emisCol(1.0f, 1.0f, 1.0f);
		material->Get(AI_MATKEY_COLOR_EMISSIVE, emisCol);
		matProperty.materialEmissiveAndPower = { emisCol.r, emisCol.g, emisCol.b, 1.0 };

		aiString path;
		if (numDiffuse > 0 && material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
		{
			std::string img(path.data);
			img = img.substr(0, img.find_last_of(".")).append(".da");
			strcpy_s(matProperty.diffuseTex, img.c_str());
		}
		if (numBump > 0 && material->GetTexture(aiTextureType_HEIGHT, 0, &path) == AI_SUCCESS)
		{
			std::string img(path.data);
			img = img.substr(0, img.find_last_of(".")).append(".da");
			strcpy_s(matProperty.bumpTex, img.c_str());
		}
		if (numSpecular > 0 && material->GetTexture(aiTextureType_SPECULAR, 0, &path) == AI_SUCCESS)
		{
			std::string img(path.data);
			img = img.substr(0, img.find_last_of(".")).append(".da");
			strcpy_s(matProperty.specularTex, img.c_str());
		}
		if (numMask > 0 && material->GetTexture(aiTextureType_OPACITY, 0, &path) == AI_SUCCESS)
		{
			std::string img(path.data);
			img = img.substr(0, img.find_last_of(".")).append(".da");
			strcpy_s(matProperty.maskTex, img.c_str());
		}
	}

	std::vector<MeshEntry> entries;
	std::vector<MeshEntry> transparentEntries;

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
		if (matProperties[entry.materialIndex].maskTex[0])
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
	std::vector<int> baseVertices;
	indiceCounts.reserve(entries.size());
	baseIndices.reserve(entries.size());
	baseVertices.reserve(entries.size());


	for (unsigned int i = 0; i < entries.size(); i++)
	{
		const MeshEntry& entry = entries[i];

		indiceCounts.push_back(entry.numIndices);
		baseIndices.push_back(entry.baseIndex * sizeof(unsigned int));
		baseVertices.push_back(entry.baseVertex);
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
			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
		}

		for (unsigned int j = 0; j < numVertices; ++j)
		{
			vertices.emplace_back(
				mesh->mVertices[j],
				mesh->HasTextureCoords(0) ? mesh->mTextureCoords[0][j] : aiVector3D(0.0f),
				mesh->HasNormals() ? mesh->mNormals[j] : aiVector3D(0.0f),
				mesh->HasTangentsAndBitangents() ? mesh->mTangents[j] : aiVector3D(0.0f),
				mesh->HasTangentsAndBitangents() ? mesh->mBitangents[j] : aiVector3D(0.0f),
				entry.materialIndex);
		}
	}
	aiReleaseImport(scene);

	std::ofstream file;
	file.open(dstFilePath.c_str(), std::ios::out | std::ios::binary);

	int numOpague = (int) transparentEntries.size();
	file.write(reinterpret_cast<const char*>(&numOpague), sizeof(int));
	writeVector(file, indiceCounts);
	writeVector(file, baseIndices);
	writeVector(file, baseVertices);
	writeVector(file, matProperties);
	writeVector(file, indices);
	writeVector(file, vertices);

	printf("%f %f %f %i %i %i\n", vertices[0].position.x, vertices[0].position.y, vertices[0].position.z, indices[0], indices[1], indices[2]);

	file.close();
}
}

void ModelProcessor::process(const char* inResourcePath, const char* outResourcePath)
{
	writeRaw(inResourcePath, outResourcePath, true);
}