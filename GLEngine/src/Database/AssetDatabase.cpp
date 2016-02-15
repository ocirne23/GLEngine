#include "Database/AssetDatabase.h"

#include "Database/AssetDatabaseEntry.h"
#include "Database/Assets/IAsset.h"
#include "Utils/FileUtils.h"

#include <assert.h>

void AssetDatabase::createNew(const eastl::string& a_filePath)
{
	assert(m_openMode == EOpenMode::UNOPENED);
	
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

bool AssetDatabase::openExisting(const eastl::string& a_filePath)
{
	assert(m_openMode == EOpenMode::UNOPENED);
	
	m_file.open(a_filePath.c_str(), std::ios::in | std::ios::binary);
	if (m_file.is_open())
	{
		m_openMode = EOpenMode::READ;
	}
	else
	{
		print("Could not find AssetDatabase: %s, did you run the resource builder?\n", a_filePath.c_str());
		return false;
	}

	// Get the file size
	m_file.seekg(0, std::ios::end);
	uint64 fileSize = m_file.tellg();
	
	// Read the position and size of the asset table
	uint64 assetTablePos, assetTableByteSize;
	m_file.seekg(0, std::ios::beg);
	m_file.read(rcast<char*>(&assetTablePos), sizeof(assetTablePos));
	m_file.read(rcast<char*>(&assetTableByteSize), sizeof(assetTableByteSize));

	AssetDatabaseEntry assetTableEntry(m_file, assetTablePos, assetTableByteSize);
	uint assetTableNumElements;
	assetTableEntry.readVal(assetTableNumElements);
	print("Opening DB: %s, num assets: %i filesize: %i MB\n", a_filePath.c_str(), assetTableNumElements, fileSize / 1024 / 1024);
	for (uint i = 0; i < assetTableNumElements; ++i)
	{
		eastl::string filePath;
		uint64 filePos, byteSize;
		assetTableEntry.readString(filePath);
		assetTableEntry.readVal(filePos);
		assetTableEntry.readVal(byteSize);
		AssetDatabaseEntry entry(m_file, filePos, byteSize);
		m_writtenAssets.insert({filePath, entry});
	}
	return true;
}

void AssetDatabase::addAsset(const eastl::string& a_databaseEntryName, owner<IAsset*> a_asset)
{
	const auto unwrittenIt = m_loadedAssets.find(a_databaseEntryName);
	const auto writtenIt = m_writtenAssets.find(a_databaseEntryName);
	if (unwrittenIt != m_loadedAssets.end() || writtenIt != m_writtenAssets.end())
	{
		print("Asset with name: %s already exists\n", a_databaseEntryName.c_str());
		assert(false);
	}
	else
	{
		m_loadedAssets.insert({a_databaseEntryName, a_asset});
	}
}

IAsset* AssetDatabase::loadAsset(const eastl::string& a_databaseEntryName, EAssetType a_type)
{
	assert(m_openMode == EOpenMode::READ);
	
	// If asset has already been loaded, return existing instance
	auto unwrittenIt = m_loadedAssets.find(a_databaseEntryName);
	if (unwrittenIt != m_loadedAssets.end())
		return unwrittenIt->second;

	auto writtenIt = m_writtenAssets.find(a_databaseEntryName);
	if (writtenIt != m_writtenAssets.end())
	{
		IAsset* asset = IAsset::create(a_type);
		asset->read(writtenIt->second);
		m_loadedAssets.insert({a_databaseEntryName, asset});
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

void AssetDatabase::writeAndClose()
{
	assert(m_openMode == EOpenMode::WRITE);
	// Write any remaining unwritten assets
	writeLoadedAssets();

	// Get the position and size of the asset table
	m_file.seekg(0, std::ios::end);
	uint64 assetTablePos = m_file.tellg();
	uint64 assetTableByteSize = AssetDatabaseEntry::getValWriteSize(uint(m_writtenAssets.size()));
	for (const auto& pair : m_writtenAssets)
	{
		assetTableByteSize += AssetDatabaseEntry::getStringWriteSize(pair.first);
		assetTableByteSize += AssetDatabaseEntry::getValWriteSize(pair.second.getFileStartPos());
		assetTableByteSize += AssetDatabaseEntry::getValWriteSize(pair.second.getTotalSize());
	}
	// Write the position and size of the asset table at the beginning of the asset file (overwriting placeholders)
	m_file.seekp(0, std::ios::beg);
	m_file.write(rcast<const char*>(&assetTablePos), sizeof(assetTablePos));
	m_file.write(rcast<const char*>(&assetTableByteSize), sizeof(assetTableByteSize));

	// Create an entry object to make it easy to write to the file.
	AssetDatabaseEntry assetTableEntry(m_file, assetTablePos, assetTableByteSize);
	// Write the number of elements in the table
	assetTableEntry.writeVal(uint(m_writtenAssets.size()));
	// For ever asset, write the file path, start byte position in the file, and the size in bytes
	for (const auto& pair : m_writtenAssets)
	{
		assetTableEntry.writeString(pair.first);
		assetTableEntry.writeVal(pair.second.getFileStartPos());
		assetTableEntry.writeVal(pair.second.getTotalSize());
	}
	// Close the file since nothing should be written after the asset table
	m_file.close();
	m_openMode = EOpenMode::UNOPENED;
}

void AssetDatabase::unloadAsset(const eastl::string& a_databaseEntryName)
{
	auto it = m_loadedAssets.find(a_databaseEntryName);
	if (it != m_loadedAssets.end())
	{
		IAsset* asset = it->second;
		delete asset;
		m_loadedAssets.erase(it);
	}
}

void AssetDatabase::unloadAsset(IAsset* a_asset)
{
	for (auto it = m_loadedAssets.begin(); it != m_loadedAssets.end(); ++it)
	{
		owner<IAsset*> asset = it->second;
		if (asset == a_asset)
		{
			m_loadedAssets.erase(it);
			delete asset;
			return;
		}
	}
}

bool AssetDatabase::hasAsset(const eastl::string& a_databaseEntryName) const
{
	auto writtenIt = m_writtenAssets.find(a_databaseEntryName);
	auto loadedIt = m_loadedAssets.find(a_databaseEntryName);
	bool found = (writtenIt != m_writtenAssets.end() || loadedIt != m_loadedAssets.end());
	return found; 
}

eastl::vector<eastl::string> AssetDatabase::listAssets() const
{
	eastl::vector<eastl::string> result;
	result.reserve(m_loadedAssets.size());
	for (const auto& it : m_writtenAssets)
		result.push_back(it.first);
	return result;
}

