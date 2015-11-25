#pragma once

#include "BuilderCore.h"
#include "Utils/FileUtils.h"

#include <fstream>
#include <vector>
#include <unordered_map>

class IAsset;

enum { MAX_FILEPATH_LENGTH = 255 };

class AssetDatabase
{
public:
	

public:
	AssetDatabase();
	~AssetDatabase();

	void addAsset(IAsset* asset);
	IAsset* getAsset(const std::string& databaseEntryName);
	std::vector<IAsset*> getAllAssetsOfType(EAssetType type);

private:

	std::unordered_map<std::string, IAsset*> m_unwrittenAssets;
};