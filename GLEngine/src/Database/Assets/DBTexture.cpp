#include "Database/Assets/DBTexture.h"

#include "stbi/stb_image.h"
#include "stbi/stb_image_write.h"

#include <assert.h>

#define IMAGE_DATA_COMPRESSED 1

BEGIN_UNNAMED_NAMESPACE()

void setPNGCompressedData(void *context, void *data, int size)
{
	DBTexture* tex = rcast<DBTexture*>(context);
	tex->setCompressedData(rcast<byte*>(data), size);
}

uint getPixColSize(DBTexture::EFormat a_format)
{
	switch (a_format)
	{
	case DBTexture::EFormat::BYTE:
		return 1;
	case DBTexture::EFormat::FLOAT:
		return 4;
	default:
		assert(false);
		return 0;
	}
}

END_UNNAMED_NAMESPACE()

void DBTexture::createNew(uint a_width, uint a_height, uint a_numComp, EFormat a_format, const byte* a_data)
{
	m_width = a_width;
	m_height = a_height;
	m_numComp = a_numComp;
	m_format = a_format;
	m_pixColSize = getPixColSize(a_format);

	uint dataSize = m_width * m_height * m_numComp * m_pixColSize;
	m_rawData.resize(dataSize);

	if (a_data)
		memcpy(m_rawData.data(), a_data, dataSize);

	m_compressedDataUpToDate = false;
}

#include "Utils/FileHandle.h"

void DBTexture::loadFromFile(const eastl::string& a_filePath, EFormat a_format, uint a_forcedNumComp)
{
	int w, h, c;
	byte* textureData = NULL;

	switch (a_format)
	{
	case EFormat::BYTE:  textureData = stbi_load(a_filePath.c_str(), &w, &h, &c, a_forcedNumComp); break;
	case EFormat::FLOAT: textureData = rcast<byte*>(stbi_loadf(a_filePath.c_str(), &w, &h, &c, a_forcedNumComp)); break;
	default: assert(false); break;
	}
	if (!textureData)
	{
		print("FAILED TO LOAD IMAGE: %s \n", a_filePath.c_str());
		assert(false);
		return;
	}

	m_format = a_format;
	m_pixColSize = getPixColSize(m_format);
	m_width = uint(w);
	m_height = uint(h);
	m_numComp = a_forcedNumComp ? a_forcedNumComp : uint(c); // If a number of components was forced, use that, otherwise use the number of components in the image.

	uint dataSize = m_width * m_height * m_numComp * m_pixColSize;
	m_rawData.resize(dataSize);
	memcpy(m_rawData.data(), textureData, dataSize);
	stbi_image_free(textureData);

	m_compressedDataUpToDate = false;
}

uint64 DBTexture::getByteSize() const
{
	uint64 totalSize = 0;
	totalSize += AssetDatabaseEntry::getValWriteSize(m_width);
	totalSize += AssetDatabaseEntry::getValWriteSize(m_height);
	totalSize += AssetDatabaseEntry::getValWriteSize(m_numComp);
	totalSize += AssetDatabaseEntry::getValWriteSize(m_format);
#if IMAGE_DATA_COMPRESSED
	if (!m_compressedDataUpToDate)
		ccast<DBTexture*>(this)->writeRawToCompressed(); // Mutation in const func!
	totalSize += AssetDatabaseEntry::getVectorWriteSize(m_compressedData);
#else
	totalSize += AssetDatabaseEntry::getVectorWriteSize(m_rawData);
#endif
	return totalSize;
}

void DBTexture::write(AssetDatabaseEntry& entry)
{
	entry.writeVal(m_width);
	entry.writeVal(m_height);
	entry.writeVal(m_numComp);
	entry.writeVal(m_format);
#if IMAGE_DATA_COMPRESSED
	if (!m_compressedDataUpToDate)
		writeRawToCompressed();
	entry.writeVector(m_compressedData);
#else
	entry.writeVector(m_rawData);
#endif
}

void DBTexture::read(AssetDatabaseEntry& entry)
{
	entry.readVal(m_width);
	entry.readVal(m_height);
	entry.readVal(m_numComp);
	entry.readVal(m_format);
	m_pixColSize = getPixColSize(m_format);

#if IMAGE_DATA_COMPRESSED
	entry.readVector(m_compressedData);
	writeCompressedToRaw();
	m_compressedData.clear();
#else
	entry.readVector(m_rawData);
#endif
}

void DBTexture::writeRawToCompressed()
{
	stbi_write_png_to_func(setPNGCompressedData, this, m_width, m_height, m_numComp, m_rawData.data(), 0);
	m_compressedDataUpToDate = true;
}

void DBTexture::writeCompressedToRaw()
{
	int w = m_width, h = m_height, n = m_numComp;
	byte* data = stbi_load_from_memory(m_compressedData.data(), int(m_compressedData.size_bytes()), &w, &h, &n, m_numComp);
	assert(w == m_width && h == m_height && n == m_numComp);

	uint dataSize = m_width * m_height * m_numComp * m_pixColSize;
	m_rawData.resize(dataSize);
	memcpy(m_rawData.data(), data, dataSize);
	stbi_image_free(data);
}

void DBTexture::setCompressedData(byte* a_data, uint64 a_size)
{
	m_compressedData.resize(a_size);
	memcpy(m_compressedData.data(), a_data, a_size);
}
