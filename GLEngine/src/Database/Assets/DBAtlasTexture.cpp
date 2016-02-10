#include "Database/Assets/DBAtlasTexture.h"

#include "Database/Assets/DBAtlasRegion.h"

#include <assert.h>

DBAtlasTexture::DBAtlasTexture(uint a_width, uint a_height, uint a_numComponents, uint a_numMipMaps)
	: m_numMipMaps(a_numMipMaps), m_texture(a_width, a_height, a_numComponents, DBTexture::EFormat::BYTE)
{
	byte red[] = {255, 0, 0, 255};
	// Initialize atlas color to red
	for (uint x = 0; x < m_texture.getWidth(); ++x)
		for (uint y = 0; y < m_texture.getHeight(); ++y)
			m_texture.setPixel(x, y, red);
}

void DBAtlasTexture::writeRegionTexture(const DBAtlasRegion& region)
{
	uint regionXPos = region.m_atlasPosition.xPos;
	uint regionYPos = region.m_atlasPosition.yPos;
	uint regionWidth = region.m_atlasPosition.width;
	uint regionHeight = region.m_atlasPosition.height;
	uint padding = m_numMipMaps * m_numMipMaps;

	// If the region fills the entire atlas, just load that image into the atlas.
	if (regionWidth == m_texture.getWidth() && regionHeight == m_texture.getHeight())
	{
		assert(regionXPos == 0 && regionYPos == 0);
		m_texture.loadImage(region.m_filePath, DBTexture::EFormat::BYTE, m_texture.getNumComponents());
		return;
	}

	// Load the texture and write the pixels into the atlas
	DBTexture regionTexture(region.m_filePath, DBTexture::EFormat::BYTE, m_texture.getNumComponents());
	assert(regionTexture.getWidth() == regionWidth);
	assert(regionTexture.getHeight() == regionHeight);
	assert(regionXPos < m_texture.getWidth());
	assert((regionXPos + regionWidth) <= m_texture.getWidth());
	assert(regionYPos < m_texture.getHeight());
	assert((regionYPos + regionHeight) <= m_texture.getHeight());

	// NOTE: can optimize a whole lot
	byte pixel[] = {0, 0, 0, 0};
	// center
	for (uint x = 0; x < regionWidth; ++x)
	{
		for (uint y = 0; y < regionHeight; ++y)
		{
			regionTexture.getPixel(x, y, pixel);
			m_texture.setPixel(regionXPos + x, regionYPos + y, pixel);
		}
	}
	// All the loops below stretch out the color of the border of the image for padding amount of pixels
	// top
	for (uint x = 0; x < regionWidth; ++x)
	{
		regionTexture.getPixel(x, 0, pixel);
		for (uint y = 0; y < padding; ++y)
		{
			m_texture.setPixel(regionXPos + x, regionYPos - y - 1, pixel);
		}
	}
	//bottom
	for (uint x = 0; x < regionWidth; ++x)
	{
		regionTexture.getPixel(x, regionHeight - 1, pixel);
		for (uint y = 0; y < padding; ++y)
		{
			m_texture.setPixel(regionXPos + x, regionYPos + y + regionHeight, pixel);
		}
	}
	//left
	for (uint x = 0; x < padding; ++x)
	{
		for (uint y = 0; y < regionHeight; ++y)
		{
			regionTexture.getPixel(0, y, pixel);
			m_texture.setPixel(regionXPos - x - 1, regionYPos + y, pixel);
		}
	}
	//right
	for (uint x = 0; x < padding; ++x)
	{
		for (uint y = 0; y < regionHeight; ++y)
		{
			regionTexture.getPixel(regionWidth - 1, y, pixel);
			m_texture.setPixel(regionXPos + x + regionWidth, regionYPos + y, pixel);
		}
	}
	// topLeft
	regionTexture.getPixel(0, 0, pixel);
	for (uint x = 0; x < padding; ++x)
	{
		for (uint y = 0; y < padding; ++y)
		{
			m_texture.setPixel(regionXPos - x - 1, regionYPos - y - 1, pixel);
		}
	}
	// topRight
	regionTexture.getPixel(regionWidth - 1, 0, pixel);
	for (uint x = 0; x < padding; ++x)
	{
		for (uint y = 0; y < padding; ++y)
		{
			m_texture.setPixel(regionXPos + x + regionWidth, regionYPos - y - 1, pixel);
		}
	}
	// bottomLeft
	regionTexture.getPixel(0, regionHeight - 1, pixel);
	for (uint x = 0; x < padding; ++x)
	{
		for (uint y = 0; y < padding; ++y)
		{
			m_texture.setPixel(regionXPos - x - 1, regionYPos + y + regionHeight, pixel);
		}
	}
	// bottomRight
	regionTexture.getPixel(regionWidth - 1, regionHeight - 1, pixel);
	for (uint x = 0; x < padding; ++x)
	{
		for (uint y = 0; y < padding; ++y)
		{
			m_texture.setPixel(regionXPos + x + regionWidth, regionYPos + y + regionHeight, pixel);
		}
	}
}

uint64 DBAtlasTexture::getByteSize() const
{
	uint64 totalSize = 0;
	totalSize += AssetDatabaseEntry::getValWriteSize(m_numMipMaps);
	totalSize += m_texture.getByteSize();
	return totalSize;
}

void DBAtlasTexture::write(AssetDatabaseEntry& entry)
{
	entry.writeVal(m_numMipMaps);
	m_texture.write(entry);
}

void DBAtlasTexture::read(AssetDatabaseEntry& entry)
{
	entry.readVal(m_numMipMaps);
	m_texture.read(entry);
}