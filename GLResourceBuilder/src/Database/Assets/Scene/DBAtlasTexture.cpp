#include "Database/Assets/Scene/DBAtlasTexture.h"

#include "Database/Assets/Scene/DBAtlasRegion.h"
#include "Utils/stb_image.h"

#include <assert.h>

BEGIN_UNNAMED_NAMESPACE()

inline void setPixel(byte* a_data, uint a_width, uint a_height, uint a_x, uint a_y, uint a_numComponents, const byte* a_pixelData)
{
	memcpy(a_data + ((a_width * a_y) + a_x) * a_numComponents, a_pixelData, a_numComponents);
}

inline void getPixel(const byte* a_data, uint a_width, uint a_height, uint a_x, uint a_y, uint a_numComponents, byte* a_outPixelData)
{
	memcpy(a_outPixelData, a_data + ((a_width * a_y) + a_x) * a_numComponents, a_numComponents);
}

END_UNNAMED_NAMESPACE()

DBAtlasTexture::DBAtlasTexture(uint a_width, uint a_height, uint a_numComponents, uint a_numMipMaps)
	: m_width(a_width), m_height(a_height), m_numComponents(a_numComponents), m_numMipMaps(a_numMipMaps)
{
	m_atlasTextureData.resize(m_width * m_height * m_numComponents);

	byte red[] = {255, 0, 0, 255};
	// Initialize atlas color to red
	for (uint xa = 0; xa < m_width; ++xa)
		for (uint ya = 0; ya < m_height; ++ya)
			setPixel(&m_atlasTextureData[0], m_width, m_height, xa, ya, m_numComponents, red);
}

void DBAtlasTexture::setRegion(const DBAtlasRegion& region)
{
	uint x = region.m_atlasPosition.xPos;
	uint y = region.m_atlasPosition.yPos;
	uint width = region.m_atlasPosition.width;
	uint height = region.m_atlasPosition.height;
	uint padding = m_numMipMaps * m_numMipMaps;
	byte* atlasTextureData = &m_atlasTextureData[0];

	int w, h, c; // maybe double check
	const byte* textureData = stbi_load(region.m_filePath.c_str(), &w, &h, &c, m_numComponents);
	if (!textureData)
	{
		print("FAILED TO LOAD IMAGE: %s \n", region.m_filePath.c_str());
		assert(false);
		return;
	}

	assert(region.m_numComp == m_numComponents);
	assert(x < m_width);
	assert((x + width) <= m_width);
	assert(y < m_height);
	assert((y + height) <= m_height);

	if (width == m_width && height == m_height)
	{
		memcpy(atlasTextureData, textureData, width * height * m_numComponents);
	}
	else
	{	// NOTE: can optimize a whole lot
		byte pixel[] = {0, 0, 0, 0};
		// center
		for (uint xPix = 0; xPix < width; ++xPix)
		{
			for (uint yPix = 0; yPix < height; ++yPix)
			{
				getPixel(textureData, width, height, xPix, yPix, m_numComponents, pixel);
				setPixel(atlasTextureData, m_width, m_height, x + xPix, y + yPix, m_numComponents, pixel);
			}
		}
		// top
		for (uint xPix = 0; xPix < width; ++xPix)
		{
			getPixel(textureData, width, height, xPix, 0, m_numComponents, pixel);
			for (uint yPix = 0; yPix < padding; ++yPix)
			{
				setPixel(atlasTextureData, m_width, m_height, x + xPix, y - yPix - 1, m_numComponents, pixel);
			}
		}
		//bottom
		for (uint xPix = 0; xPix < width; ++xPix)
		{
			getPixel(textureData, width, height, xPix, height - 1, m_numComponents, pixel);
			for (uint yPix = 0; yPix < padding; ++yPix)
			{
				setPixel(atlasTextureData, m_width, m_height, x + xPix, y + yPix + height, m_numComponents, pixel);
			}
		}
		//left
		for (uint xPix = 0; xPix < padding; ++xPix)
		{
			for (uint yPix = 0; yPix < height; ++yPix)
			{
				getPixel(textureData, width, height, 0, yPix, m_numComponents, pixel);
				setPixel(atlasTextureData, m_width, m_height, x - xPix - 1, y + yPix, m_numComponents, pixel);
			}
		}
		//right
		for (uint xPix = 0; xPix < padding; ++xPix)
		{
			for (uint yPix = 0; yPix < height; ++yPix)
			{
				getPixel(textureData, width, height, width - 1, yPix, m_numComponents, pixel);
				setPixel(atlasTextureData, m_width, m_height, x + xPix + width, y + yPix, m_numComponents, pixel);
			}
		}
		// topLeft
		getPixel(textureData, width, height, 0, 0, m_numComponents, pixel);
		for (uint xPix = 0; xPix < padding; ++xPix)
		{
			for (uint yPix = 0; yPix < padding; ++yPix)
			{
				setPixel(atlasTextureData, m_width, m_height, x - xPix - 1, y - yPix - 1, m_numComponents, pixel);
			}
		}
		// topRight
		getPixel(textureData, width, height, width - 1, 0, m_numComponents, pixel);
		for (uint xPix = 0; xPix < padding; ++xPix)
		{
			for (uint yPix = 0; yPix < padding; ++yPix)
			{
				setPixel(atlasTextureData, m_width, m_height, x + xPix + width, y - yPix - 1, m_numComponents, pixel);
			}
		}
		// bottomLeft
		getPixel(textureData, width, height, 0, height - 1, m_numComponents, pixel);
		for (uint xPix = 0; xPix < padding; ++xPix)
		{
			for (uint yPix = 0; yPix < padding; ++yPix)
			{
				setPixel(atlasTextureData, m_width, m_height, x - xPix - 1, y + yPix + height, m_numComponents, pixel);
			}
		}
		// bottomRight
		getPixel(textureData, width, height, width - 1, height - 1, m_numComponents, pixel);
		for (uint xPix = 0; xPix < padding; ++xPix)
		{
			for (uint yPix = 0; yPix < padding; ++yPix)
			{
				setPixel(atlasTextureData, m_width, m_height, x + xPix + width, y + yPix + height, m_numComponents, pixel);
			}
		}
	}
	stbi_image_free((void*) textureData);
}

uint64 DBAtlasTexture::getByteSize() const
{
	uint64 totalSize = 0;
	totalSize += sizeof(m_width);
	totalSize += sizeof(m_height);
	totalSize += sizeof(m_numComponents);
	totalSize += sizeof(m_numMipMaps);
	totalSize += AssetDatabaseEntry::getVectorWriteSize(m_atlasTextureData);
	return totalSize;
}

void DBAtlasTexture::write(AssetDatabaseEntry& entry)
{
	entry.writeVal(m_width);
	entry.writeVal(m_height);
	entry.writeVal(m_numComponents);
	entry.writeVal(m_numMipMaps);
	entry.writeVector(m_atlasTextureData);
}

void DBAtlasTexture::read(AssetDatabaseEntry& entry)
{

}