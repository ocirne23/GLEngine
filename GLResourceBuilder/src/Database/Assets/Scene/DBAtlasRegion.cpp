#include "Database/Assets/Scene/DBAtlasRegion.h"

#include "Utils/stb_image.h"

uint64 DBAtlasRegion::getByteSize() const 
{
	uint64 totalSize = 0;
	totalSize += AssetDatabaseEntry::getStringWriteSize(m_filePath);
	totalSize += sizeof(m_width);
	totalSize += sizeof(m_height);
	totalSize += sizeof(m_numComp);
	totalSize += sizeof(m_atlasPosition);
	return totalSize;
}

void DBAtlasRegion::write(AssetDatabaseEntry& entry)
{
	entry.writeString(m_filePath);
	entry.writeVal(m_width);
	entry.writeVal(m_height);
	entry.writeVal(m_numComp);
	entry.writeVal(m_atlasPosition);
}

void DBAtlasRegion::read(AssetDatabaseEntry& entry)
{
	
}

void DBAtlasRegion::loadInfo(const std::string& a_filePath)
{
	int result = stbi_info(a_filePath.c_str(), (int*) &m_width, (int*) &m_height, (int*) &m_numComp);
	if (result)
		m_filePath = a_filePath;
	else
	{
		print("File not found: %s\n", a_filePath.c_str());
		assert(false);
	}
	assert(result);
}
