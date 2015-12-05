#include "Database/Processors/SceneProcessor.h"

#include "Core.h"
#include "Database/Assets/EAssetType.h"
#include "Database/AssetDatabase.h"
#include "Database/Assets/DBScene.h"
#include "Database/Assets/DBMaterial.h"
#include "Database/Assets/DBMesh.h"
#include "Database/Utils/stb_image.h"
#include "Database/Utils/AtlasBuilder.h"

#include "Utils/FileUtils.h"

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <fstream>
#include <iostream>

bool SceneProcessor::process(const eastl::string& a_inResourcePath, AssetDatabase& a_assetDatabase)
{
	const uint flags = 0
		| aiPostProcessSteps::aiProcess_Triangulate
		| aiPostProcessSteps::aiProcess_CalcTangentSpace
		| aiPostProcessSteps::aiProcess_GenNormals
		| aiPostProcessSteps::aiProcess_RemoveRedundantMaterials
		| aiPostProcessSteps::aiProcess_FlipUVs; // Flip uv's because OpenGL

	const aiScene* assimpScene = aiImportFile(a_inResourcePath.c_str(), flags);
	
	eastl::string fileName = FileUtils::getFileNameFromPath(a_inResourcePath);
	a_assetDatabase.addAsset(fileName, new DBScene(*assimpScene, FileUtils::getFolderPathForFile(a_inResourcePath)));

	aiReleaseImport(assimpScene);
	return true;
}