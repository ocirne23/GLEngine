#include "Database/AssetDatabase.h"

#include "Database/AssetDatabaseEntry.h"
#include "Database/Assets/IAsset.h"
#include "Database/Utils/CRC64.h"
#include "Utils/FileUtils.h"

#include <assert.h>

void AssetDatabase::createNew(const eastl::string& a_filePath)
{
	assert(m_openMode == EOpenMode::UNOPENED);
	
	print("Creating new database\n");
	m_openMode = EOpenMode::WRITE;
	m_file.open(a_filePath.c_str(), std::ios::out | std::ios::binary);
	assert(m_file.is_open());
	
	// Ensure there is dummy data at the start of the file to hold asset table info
	uint64 assetTablePos = 0;
	uint64 assetTableByteSize = 0;
	m_file.write(reinterpret_cast<const char*>(&assetTablePos), sizeof(assetTablePos));
	m_file.write(reinterpret_cast<const char*>(&assetTableByteSize), sizeof(assetTableByteSize));
	m_assetWritePos = sizeof(uint64) * 2;
}

void AssetDatabase::openExisting(const eastl::string& a_filePath)
{
	assert(m_openMode == EOpenMode::UNOPENED);
	
	print("Opening existing database\n");
	m_openMode = EOpenMode::READ;
	m_file.open(a_filePath.c_str(), std::ios::in | std::ios::binary);
	assert(m_file.is_open());

	// Tell the file size
	m_file.seekg(0, std::ios::end);
	uint64 fileSize = m_file.tellg();
	
	// Load the asset table
	uint64 assetTablePos, assetTableByteSize;
	m_file.seekg(0, std::ios::beg);
	m_file.read(reinterpret_cast<char*>(&assetTablePos), sizeof(assetTablePos));
	m_file.read(reinterpret_cast<char*>(&assetTableByteSize), sizeof(assetTableByteSize));

	AssetDatabaseEntry assetTableEntry(m_file, assetTablePos, assetTableByteSize);

	uint assetTableSize;
	assetTableEntry.readVal(assetTableSize);

	for (uint i = 0; i < assetTableSize; ++i)
	{
		uint64 filePathHash, filePos, byteSize;
		assetTableEntry.readVal(filePathHash);
		assetTableEntry.readVal(filePos);
		assetTableEntry.readVal(byteSize);
		AssetDatabaseEntry entry(m_file, filePos, byteSize);
		m_writtenAssets.insert({filePathHash, entry});
	}

	print("Opened DB, num assets: %i filesize: %iMB\n", m_writtenAssets.size(), fileSize / 1024 / 1024);
}

void AssetDatabase::addAsset(const eastl::string& a_databaseEntryName, IAsset* a_asset)
{
	uint64 hash = CRC64::getHash(a_databaseEntryName.c_str());
	const auto unwrittenIt = m_loadedAssets.find(hash);
	const auto writtenIt = m_writtenAssets.find(hash);
	if (unwrittenIt != m_loadedAssets.end() || writtenIt != m_writtenAssets.end())
	{
		print("Asset with name: %s already exists\n", a_databaseEntryName.c_str());
		assert(false);
	}
	else
	{
		m_loadedAssets.insert({hash, a_asset});
	}
}

IAsset* AssetDatabase::loadAsset(const eastl::string& a_databaseEntryName, EAssetType a_type)
{
	assert(m_openMode == EOpenMode::READ);
	uint64 hash = CRC64::getHash(a_databaseEntryName.c_str());
	
	// If asset has already been loaded, return existing instance
	auto unwrittenIt = m_loadedAssets.find(hash);
	if (unwrittenIt != m_loadedAssets.end())
		return unwrittenIt->second;

	auto writtenIt = m_writtenAssets.find(hash);
	if (writtenIt != m_writtenAssets.end())
	{
		IAsset* asset = IAsset::create(a_type);
		asset->read(writtenIt->second);
		m_loadedAssets.insert({hash, asset});
		m_writtenAssets.erase(writtenIt);
		return asset;
	}

	return NULL;
}

void AssetDatabase::writeLoadedAssets()
{
	assert(m_openMode == EOpenMode::WRITE);

	for (auto& pair : m_loadedAssets)
	{	// Create db entry to hold the asset
		uint64 size = pair.second->getByteSize();
		AssetDatabaseEntry entry(m_file, m_assetWritePos, size);
		m_assetWritePos += size;
		
		// Write the asset
		pair.second->write(entry);
		assert(entry.validateWritten());
		SAFE_DELETE(pair.second);
		m_writtenAssets.insert({pair.first, entry});
	}
	m_loadedAssets.clear();
}

void AssetDatabase::writeAssetTableAndClose()
{
	assert(m_openMode == EOpenMode::WRITE);
	// Write any remaining unwritten assets
	writeLoadedAssets();

	// Write asset table at the end of the file
	m_file.seekg(0, std::ios::end);
	uint64 assetTablePos = m_file.tellg();
	uint64 assetTableByteSize = sizeof(uint) + sizeof(uint64) * 3 * m_writtenAssets.size();
	AssetDatabaseEntry assetTableEntry(m_file, assetTablePos, assetTableByteSize);

	// Write information about every file in the db
	assetTableEntry.writeVal((uint) m_writtenAssets.size());
	for (const auto& pair : m_writtenAssets)
	{
		uint64 filePathHash = pair.first;
		uint64 filePos = pair.second.getFileStartPos();
		uint64 byteSize = pair.second.getTotalSize();
		assetTableEntry.writeVal(filePathHash);
		assetTableEntry.writeVal(filePos);
		assetTableEntry.writeVal(byteSize);
	}

	// Write location and size of the asset table at the start of the database file
	m_file.seekp(0, std::ios::beg);
	m_file.write(reinterpret_cast<const char*>(&assetTablePos), sizeof(assetTablePos));
	m_file.write(reinterpret_cast<const char*>(&assetTableByteSize), sizeof(assetTableByteSize));
	
	// Close the file since nothing should be written after the asset table
	m_file.close();
	m_openMode = EOpenMode::UNOPENED;
}

void AssetDatabase::unloadAsset(const eastl::string& a_databaseEntryName)
{
	uint64 hash = CRC64::getHash(a_databaseEntryName.c_str());
	auto it = m_loadedAssets.find(hash);
	if (it != m_loadedAssets.end())
	{
		IAsset* asset = it->second;
		delete asset;
		m_loadedAssets.erase(it);
	}
}

void AssetDatabase::unloadAsset(IAsset* a_asset)
{
	for (eastl::hash_map<uint64, IAsset*>::iterator it = m_loadedAssets.begin(); it != m_loadedAssets.end(); ++it)
	{
		IAsset* asset = it->second;
		if (asset == a_asset)
		{
			m_loadedAssets.erase(it);
			delete a_asset;
			return;
		}
	}
}

