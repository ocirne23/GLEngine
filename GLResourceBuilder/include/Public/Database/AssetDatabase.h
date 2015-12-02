#pragma once

#include "BuilderCore.h"
#include "Database/AssetDatabaseEntry.h"
#include "Database/EAssetType.h"
#include "Utils/FileUtils.h"
#include "EASTL/hash_map.h"
#include "EASTL/string.h"

#include <fstream>

class IAsset;

class AssetDatabase
{
public:
	AssetDatabase() {}
	~AssetDatabase() {}

	// WRITE api
	void createNew(const eastl::string& filePath);
	void addAsset(const eastl::string& databaseEntryName, IAsset* asset);
	void writeUnwrittenAssets();
	void writeAssetTableAndClose();

	// READ api
	void openExisting(const eastl::string& filePath);
	IAsset* loadAsset(const eastl::string& databaseEntryName, EAssetType type);

private:

	enum class EOpenMode
	{
		READ,
		WRITE,
		UNOPENED
	};

private:

	EOpenMode m_openMode = EOpenMode::UNOPENED;
	std::fstream m_file;
	uint64 m_assetWritePos = 0;
	eastl::hash_map<uint64, IAsset*> m_unwrittenAssets;
	eastl::hash_map<uint64, AssetDatabaseEntry> m_writtenAssets;
};