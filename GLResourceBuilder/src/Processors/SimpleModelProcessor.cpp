#include "SimpleModelProcessor.h"

#include <vector>
#include <fstream>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace
{
struct vec4 { float x, y, z, w; };
struct vec3 { float x, y, z; };
struct vec2 { float x, y; };

struct MeshEntry
{
	unsigned int meshIndex;
	unsigned int numIndices;
	unsigned int baseVertex;
	unsigned int baseIndex;
	unsigned int materialIndex;
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

	std::vector<MeshEntry> entries;

	entries.reserve(scene->mNumMeshes);

	unsigned int numVerticesInScene = 0;
	unsigned int numIndicesInScene = 0;

	for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
	{
		MeshEntry entry;
		entry.meshIndex = i;
		entry.materialIndex = scene->mMeshes[i]->mMaterialIndex;
		entry.numIndices = scene->mMeshes[i]->mNumFaces * 3;

		entry.baseVertex = numVerticesInScene;
		entry.baseIndex = numIndicesInScene;
		numVerticesInScene += scene->mMeshes[i]->mNumVertices;
		numIndicesInScene += entry.numIndices;
		entries.push_back(entry);
	}

	std::vector<vec3> vertices;
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
			vertices.emplace_back(*((vec3*) &mesh->mVertices[j]));
		}
	}
	aiReleaseImport(scene);

	std::ofstream file;
	file.open(dstFilePath.c_str(), std::ios::out | std::ios::binary);

	writeVector(file, indiceCounts);
	writeVector(file, baseIndices);
	writeVector(file, baseVertices);
	writeVector(file, indices);
	writeVector(file, vertices);

	printf("%f %f %f %i %i %i\n", vertices[0].x, vertices[0].y, vertices[0].z, indices[0], indices[1], indices[2]);

	file.close();
}
}

void SimpleModelProcessor::process(const char* inResourcePath, const char* outResourcePath)
{
	writeRaw(inResourcePath, outResourcePath, false);
}