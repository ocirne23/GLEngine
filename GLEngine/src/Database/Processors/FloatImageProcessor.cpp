#include "Database/Processors/FloatImageProcessor.h"

#include "Database/Assets/DBTexture.h"
#include "EASTL/string.h"
#include "Utils/FileUtils.h"

#include <assert.h>

bool FloatImageProcessor::process(const eastl::string& a_resourcePath, AssetDatabase& a_assetDatabase)
{
	DBTexture* texture = new DBTexture(a_resourcePath, DBTexture::EFormat::FLOAT);
	eastl::string fileName = FileUtils::getFileNameFromPath(a_resourcePath);
	a_assetDatabase.addAsset(fileName, texture);
	return true;
}