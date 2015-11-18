#include "Database/AssetDatabase.h"

#include "Database/IAsset.h"
#include "Utils/CRC64.h"
#include "Utils/writeVector.h"

#include <assert.h>

AssetDatabase::AssetDatabase() {}
AssetDatabase::~AssetDatabase() {}

void AssetDatabase::saveAndClose()
{
	assert(m_openMode == EOpenMode::WRITEABLE);
	// Write lookuptable at end of file
	writeVector(m_file, m_lookupTable); 
	// Write position of lookuptable at beginning of file after header and time.
	m_file.seekp(sizeof(uint) + sizeof(FileTime));
	m_file.write(reinterpret_cast<const char*>(&m_assetDataOffset), sizeof(m_assetDataOffset));
	m_file.close();
	m_openMode = EOpenMode::CLOSED;
}

void AssetDatabase::create(const char* a_filePath)
{
	assert(m_openMode == EOpenMode::CLOSED);
	m_file.open(a_filePath, std::ios::out | std::ios::binary);
	assert(m_file.is_open());

	const uint m_header = (uint) EAssetType::ASSETDB;
	const uint64 lookupTablePos = 0xFFFFFFFFFFFFFFFFull; // Placeholder
	const FileTime writeTime = getCurrentFileTime();
	
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
	assert(m_openMode == EOpenMode::WRITEABLE);
	// Todo: Check for duplicates or hash conflicts?
	m_lookupTable.push_back({CRC64::getHash(a_filePath), m_assetSizeCounter});

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

IAsset* AssetDatabase::getAsset(const char* filePath)
{
	assert(m_openMode == EOpenMode::READONLY);
	// TODO
	return NULL;
}