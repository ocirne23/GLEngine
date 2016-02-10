#include "Database/Processors/ByteImageProcessor.h"

#include "Database/Assets/DBTexture.h"
#include "Database/AssetDatabase.h"

#include <fstream>

bool ByteImageProcessor::process(const eastl::string& a_resourcePath, AssetDatabase& a_assetDatabase)
{
	DBTexture* texture = new DBTexture(a_resourcePath);
	eastl::string fileName = FileUtils::getFileNameFromPath(a_resourcePath);
	a_assetDatabase.addAsset(fileName, texture);
	return true;
}