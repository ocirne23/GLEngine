#include "Database/AssetDatabase.h"

#include "AtlasBuilder.h"
#include "Database/IAsset.h"
#include "Utils/CRC64.h"
#include "Utils/FileUtils.h"
#include "Utils/writeVector.h"

#include <assert.h>

AssetDatabase::AssetDatabase() {}
AssetDatabase::~AssetDatabase() {}


std::vector<IAsset*> AssetDatabase::getAllAssetsOfType(EAssetType a_type)
{
	std::vector<IAsset*> assets;
	for (std::pair<std::string, IAsset*>& pair : m_unwrittenAssets)
		if (pair.second->getAssetType() == a_type)
			assets.push_back(pair.second);
	return assets;
}