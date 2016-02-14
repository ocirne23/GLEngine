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

	/* Create an empty asset database with the specified name/path */
	void createNew(const eastl::string& filePath);
	/* Open an existing asset database file with the specified name/path, returns if succeeded */
	bool openExisting(const eastl::string& filePath);
	/* Add an asset to the database under the specified name, transfering ownership */
	void addAsset(const eastl::string& databaseEntryName, owner<IAsset*> asset);
	/* Write away currently loaded assets to so the memory can be freed */
	void writeLoadedAssets();
	/* Write assets and index table and close the file */
	void writeAndClose();

	//TODO: add some form of reference counter to unload assets?

	/* Load an asset with the specified name and type, the result is cached */
	IAsset* loadAsset(const eastl::string& databaseEntryName, EAssetType type);
	/* Unload a loaded/cached asset with the specified name to free it's memory */
	void unloadAsset(const eastl::string& databaseEntryName);
	/* Unload a loaded/cached asset to free it's memory */
	void unloadAsset(IAsset* asset);

	bool hasAsset(const eastl::string& databaseEntryName) const;
	bool isOpen() const { return m_openMode != EOpenMode::UNOPENED; }
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
	eastl::hash_map<eastl::string, owner<IAsset*>> m_loadedAssets;
	eastl::hash_map<eastl::string, AssetDatabaseEntry> m_writtenAssets;
};