#pragma once
/*
#include "BuilderCore.h"
#include "Database/Assets/Mesh.h"
#include <assimp/scene.h>
#include <vector>

class MeshMerger
{
public:
	void countMeshes(std::vector<uint>& a_meshCountList, const aiNode* a_node)
	{
		for (uint i = 0; i < a_node->mNumMeshes; ++i)
			a_meshCountList[a_node->mMeshes[i]]++;

		uint numChildren = a_node->mNumChildren;
		for (uint i = 0; i < numChildren; ++i)
			countMeshes(a_meshCountList, a_node->mChildren[i]);
	}

	// Get the number of occurrences for every mesh 
	std::vector<uint> countMeshes(const aiScene* a_assimpScene)
	{
		std::vector<uint> meshCountList(a_assimpScene->mNumMeshes);
		countMeshes(meshCountList, a_assimpScene->mRootNode);
		return meshCountList;
	}

	// Get the transform of the first node using a mesh with the given ID, returns if a transform was found
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

	Mesh getSingleOccurrenceMergedMesh(const aiScene* a_scene, const std::vector<uint>& a_meshCountList)
	{
		Mesh mergedMesh;
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
		mergedMesh.indices.resize(numIndices);
		mergedMesh.vertices.resize(numVertices);

		for (uint i = 0; i < a_meshCountList.size(); ++i)
		{
			if (a_meshCountList[i] == 1)
			{
				aiMatrix3x3 transform = findAbsoluteTransformForMesh(a_scene, i);
				addVerticesIndices(mergedMeshData, a_scene->mMeshes[i], transform, true);
			}
		}
		return mergedMeshData;
	}
};
*/