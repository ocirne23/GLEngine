#include "Database/Assets/DBTexture.h"

#include "Database/Utils/stb_image.h"

#include <assert.h>

BEGIN_UNNAMED_NAMESPACE()

uint getPixColSize(DBTexture::EFormat a_format)
{
	switch (a_format)
	{
	case DBTexture::EFormat::BYTE:
		return 1;
	case DBTexture::EFormat::FLOAT:
		return 4;
	}
	assert(false);
	return 0;
}

END_UNNAMED_NAMESPACE()

DBTexture::DBTexture(uint a_width, uint a_height, uint a_numComp, EFormat a_format, const byte* a_data)
	: m_width(a_width), m_height(a_height), m_numComp(a_numComp), m_format(a_format), m_pixColSize(getPixColSize(a_format))
{
	uint dataSize = m_width * m_height * m_numComp * m_pixColSize;
	m_data.resize(dataSize);
	if (a_data)
		memcpy(&m_data[0], a_data, dataSize);
}

DBTexture::DBTexture(const eastl::string& a_filePath, EFormat a_format, uint a_forcedNumComp)
{
	loadImage(a_filePath, a_format, a_forcedNumComp);
}

void DBTexture::loadImage(const eastl::string& a_filePath, EFormat a_format, uint a_forcedNumComp)
{
	int w, h, c;
	byte* textureData = NULL;
	
	if (a_format == EFormat::BYTE)
		textureData = stbi_load(a_filePath.c_str(), &w, &h, &c, a_forcedNumComp);
	else if (a_format == EFormat::FLOAT)
		textureData = (byte*) stbi_loadf(a_filePath.c_str(), &w, &h, &c, a_forcedNumComp);
	else
	{
		assert(false);
	}
	if (!textureData)
	{
		print("FAILED TO LOAD IMAGE: %s \n", a_filePath.c_str());
		assert(false);
		return;
	}

	m_format = a_format;
	m_pixColSize = getPixColSize(m_format);

	// If size was set before loading, ensure that the size of the loaded image is the same.
	if (m_width != 0 || m_height != 0) 
	{
		assert((uint) w == m_width);
		assert((uint) h == m_height);
	}
	m_width = (uint) w;
	m_height = (uint) h;

	// If a number of components was forced, use that, otherwise use the number of components in the image.
	if (a_forcedNumComp != 0)
		m_numComp = a_forcedNumComp;
	else
		m_numComp = (uint) c;

	uint dataSize = m_width * m_height * m_numComp * m_pixColSize;
	m_data.resize(dataSize);
	memcpy(&m_data[0], textureData, dataSize);

	stbi_image_free(textureData);
}

uint64 DBTexture::getByteSize() const
{
	uint64 totalSize = 0;
	totalSize += AssetDatabaseEntry::getValWriteSize(m_width);
	totalSize += AssetDatabaseEntry::getValWriteSize(m_height);
	totalSize += AssetDatabaseEntry::getValWriteSize(m_numComp);
	totalSize += AssetDatabaseEntry::getValWriteSize(m_format);
	totalSize += AssetDatabaseEntry::getVectorWriteSize(m_data);
	return totalSize;
}

void DBTexture::write(AssetDatabaseEntry& entry)
{
	entry.writeVal(m_width);
	entry.writeVal(m_height);
	entry.writeVal(m_numComp);
	entry.writeVal(m_format);
	entry.writeVector(m_data);
}

void DBTexture::read(AssetDatabaseEntry& entry)
{
	entry.readVal(m_width);
	entry.readVal(m_height);
	entry.readVal(m_numComp);
	entry.readVal(m_format);
	entry.readVector(m_data);
}
