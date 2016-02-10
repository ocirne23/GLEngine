#pragma once

#include "Core.h"
#include "Database/AssetDatabaseEntry.h"
#include "Database/Assets/EAssetType.h"
#include "Utils/FileUtils.h"
#include "EASTL/hash_map.h"
#include "EASTL/vector.h"
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
	void writeLoadedAssets();
	void writeAssetTableAndClose();

	// READ api
	bool openExisting(const eastl::string& filePath);
	
	//TODO: add some form of reference counter to unload assets
	
	IAsset* loadAsset(const eastl::string& databaseEntryName, EAssetType type);
	void unloadAsset(const eastl::string& databaseEntryName);
	void unloadAsset(IAsset* asset);

	bool isOpen() const { return m_openMode != EOpenMode::UNOPENED; }
	bool hasAsset(const eastl::string& databaseEntryName) const;
	eastl::vector<eastl::string> listAssets() const;

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
	eastl::hash_map<eastl::string, IAsset*> m_loadedAssets;
	eastl::hash_map<eastl::string, AssetDatabaseEntry> m_writtenAssets;
};