#include "Database/Assets/DBAtlasRegion.h"

#include "stbi/stb_image.h"

uint64 DBAtlasRegion::getByteSize() const 
{
	uint64 totalSize = 0;
	totalSize += AssetDatabaseEntry::getStringWriteSize(m_filePath);
	totalSize += AssetDatabaseEntry::getValWriteSize(m_texWidth);
	totalSize += AssetDatabaseEntry::getValWriteSize(m_texHeight);
	totalSize += AssetDatabaseEntry::getValWriteSize(m_numComp);
	totalSize += AssetDatabaseEntry::getValWriteSize(m_atlasPosition);
	totalSize += AssetDatabaseEntry::getValWriteSize(m_atlasMapping);
	totalSize += AssetDatabaseEntry::getValWriteSize(m_atlasIdx);
	return totalSize;
}

void DBAtlasRegion::write(AssetDatabaseEntry& entry)
{
	entry.writeString(m_filePath);
	entry.writeVal(m_texWidth);
	entry.writeVal(m_texHeight);
	entry.writeVal(m_numComp);
	entry.writeVal(m_atlasPosition);
	entry.writeVal(m_atlasMapping);
	entry.writeVal(m_atlasIdx);
}

void DBAtlasRegion::read(AssetDatabaseEntry& entry)
{
	entry.readString(m_filePath);
	entry.readVal(m_texWidth);
	entry.readVal(m_texHeight);
	entry.readVal(m_numComp);
	entry.readVal(m_atlasPosition);
	entry.readVal(m_atlasMapping);
	entry.readVal(m_atlasIdx);
}

void DBAtlasRegion::loadInfo(const eastl::string& a_filePath)
{
	const int result = stbi_info(a_filePath.c_str(), (int*) &m_texWidth, (int*) &m_texHeight, (int*) &m_numComp);
	if (result)
		m_filePath = a_filePath;
	else
		print("File not found: %s\n", a_filePath.c_str());
	assert(result);
}
