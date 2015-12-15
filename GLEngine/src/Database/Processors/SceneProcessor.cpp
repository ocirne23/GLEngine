#include "Database/Processors/SceneProcessor.h"

#include "Core.h"
#include "Database/AssetDatabase.h"
#include "Database/Assets/DBScene.h"
#include "Utils/FileUtils.h"

bool SceneProcessor::process(const eastl::string& a_inResourcePath, AssetDatabase& a_assetDatabase)
{
	a_assetDatabase.addAsset(FileUtils::getFileNameFromPath(a_inResourcePath), new DBScene(a_inResourcePath));
	return true;
}