#include "Database/Processors/FloatImageProcessor.h"

#include "Database/Assets/DBTexture.h"
#include "EASTL/string.h"
#include "Utils/FileUtils.h"

bool FloatImageProcessor::process(const eastl::string& a_resourcePath, AssetDatabase& a_assetDatabase)
{
	owner<DBTexture*> texture = new DBTexture();
	texture->loadFromFile(a_resourcePath, DBTexture::EFormat::FLOAT);
	a_assetDatabase.addAsset(FileUtils::getFileNameFromPath(a_resourcePath), texture);
	return true;
}