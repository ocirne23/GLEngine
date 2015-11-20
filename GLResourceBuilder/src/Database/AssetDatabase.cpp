#include "Database/AssetDatabase.h"

#include "AtlasBuilder.h"
#include "Database/IAsset.h"
#include "Utils/CRC64.h"
#include "Utils/FileUtils.h"
#include "Utils/writeVector.h"

#include <assert.h>

enum { LOOKUP_TABLE_POS_BYTEOFFSET = (sizeof(uint) + sizeof(FileTime)) };

AssetDatabase::AssetDatabase() {}
AssetDatabase::~AssetDatabase() {}

void AssetDatabase::saveAndClose()
{
	if (m_unwrittenAssets.size())
		flush();

	assert(m_openMode == EOpenMode::WRITEABLE);
	// Write lookuptable at end of file
	writeVector(m_file, m_fileLookupTable); 
	// Write position of lookuptable at beginning of file after header and time.
	m_file.seekp(LOOKUP_TABLE_POS_BYTEOFFSET);
	m_file.write(reinterpret_cast<const char*>(&m_assetDataOffset), sizeof(m_assetDataOffset));
	m_file.close();
	m_openMode = EOpenMode::CLOSED;
}

void AssetDatabase::flush()
{
	for (std::pair<std::string, IAsset*> entry : m_unwrittenAssets)
	{
		writeAsset(entry.first, entry.second);
		delete entry.second;
	}
	m_unwrittenAssets.clear();
}

void AssetDatabase::create(const char* a_filePath)
{
	assert(m_openMode == EOpenMode::CLOSED);
	m_file.open(a_filePath, std::ios::out | std::ios::binary);
	assert(m_file.is_open());

	const uint m_header = (uint) EAssetType::ASSETDB;
	const uint64 lookupTablePos = 0xFFFFFFFFFFFFFFFFull; // Placeholder
	const FileTime writeTime = FileUtils::getCurrentFileTime();
	
	m_file.write(reinterpret_cast<const char*>(&m_header), sizeof(uint));
	m_file.write(reinterpret_cast<const char*>(&writeTime), sizeof(FileTime));
	m_file.write(reinterpret_cast<const char*>(&lookupTablePos), sizeof(uint64));
	m_assetDataOffset = sizeof(uint) + sizeof(FileTime) + sizeof(uint64);

	m_openMode = EOpenMode::WRITEABLE;
}

void AssetDatabase::openForReading(const char* a_filePath)
{
	assert(m_openMode == EOpenMode::CLOSED);
	m_file.open(a_filePath, std::ios::in | std::ios::binary);
	// TODO

	m_openMode = EOpenMode::READONLY;
}

void AssetDatabase::close()
{
	assert(m_openMode == EOpenMode::READONLY);
	m_file.close();
	m_openMode = EOpenMode::CLOSED;
}

void AssetDatabase::addAsset(const char* a_filePath, IAsset* a_asset)
{
	m_unwrittenAssets.insert({a_filePath, a_asset});
}

IAsset* AssetDatabase::getAsset(const char* filePath)
{
	assert(m_openMode == EOpenMode::READONLY);
	return NULL;
}

void AssetDatabase::writeAsset(const std::string& a_filePath, IAsset* a_asset)
{
	assert(m_openMode == EOpenMode::WRITEABLE);
	// Todo: Check for duplicates or hash conflicts?
	m_fileLookupTable.push_back({CRC64::getHash(a_filePath.c_str()), m_assetSizeCounter});

	uint64 pos = m_file.tellp();

	EAssetType type = a_asset->getAssetType();
	m_file.write(reinterpret_cast<const char*>(&type), sizeof(type));
	uint size = a_asset->getByteSize();
	m_file.write(reinterpret_cast<const char*>(&size), sizeof(size));
	a_asset->write(m_file);
	uint64 newPos = m_file.tellp();

	m_assetSizeCounter += sizeof(type) + sizeof(size) + a_asset->getByteSize();
	assert((pos + (int) m_assetSizeCounter) == newPos);
}

IAsset* AssetDatabase::readAsset(const std::string& filePath)
{
	return NULL;
}

std::vector<IAsset*> AssetDatabase::getAllAssetsOfType(EAssetType a_type)
{
	std::vector<IAsset*> assets;
	for (std::pair<std::string, IAsset*>& pair : m_unwrittenAssets)
		if (pair.second->getAssetType() == a_type)
			assets.push_back(pair.second);
	return assets;
}

void AssetDatabase::packAtlasTextures()
{
	std::vector<AtlasTexture*> textures = (std::vector<AtlasTexture*>) getAllAssetsOfType(EAssetType::ATLAS_TEXTURE);
	std::vector<TextureAtlas*> atlases = AtlasBuilder::fitTextures(textures);
}
