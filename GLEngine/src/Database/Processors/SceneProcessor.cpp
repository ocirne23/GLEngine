#include "Database/Processors/SceneProcessor.h"

#include "Database/Assets/DBScene.h"
#include "Utils/FileUtils.h"

bool SceneProcessor::process(const eastl::string& a_inResourcePath, AssetDatabase& a_assetDatabase)
{
	owner<DBScene*> scene = new DBScene(a_inResourcePath);
	const eastl::string fileName = FileUtils::getFileNameFromPath(a_inResourcePath);
	a_assetDatabase.addAsset(fileName, scene);
	return true;
}