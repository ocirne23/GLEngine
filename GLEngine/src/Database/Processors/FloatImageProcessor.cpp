#include "Database/Processors/FloatImageProcessor.h"

#include "Database/Assets/DBTexture.h"
#include "EASTL/string.h"
#include "Utils/FileUtils.h"

bool FloatImageProcessor::process(const eastl::string& a_resourcePath, AssetDatabase& a_assetDatabase)
{
	owner<DBTexture*> texture = new DBTexture(a_resourcePath, DBTexture::EFormat::FLOAT);
	const eastl::string fileName = FileUtils::getFileNameFromPath(a_resourcePath);
	a_assetDatabase.addAsset(fileName, texture);
	return true;
}