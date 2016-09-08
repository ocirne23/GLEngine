#pragma once

#include "Database/Assets/IAsset.h"
#include "EASTL/string.h"
#include "EASTL/vector.h"
#include <glm/glm.hpp>

class DBTexture : public IAsset
{
public:

	enum class EFormat
	{
		BYTE,
		FLOAT
	};

	void createNew(uint width, uint height, uint numComp, EFormat format, const byte* data = NULL);
	void loadFromFile(const eastl::string& filePath, EFormat format = EFormat::BYTE, uint forcedNumComp = 0);

	virtual uint64 getByteSize() const override;
	virtual EAssetType getAssetType() const override { return EAssetType::TEXTURE; }
	virtual void write(AssetDatabaseEntry& entry) override;
	virtual void read(AssetDatabaseEntry& entry) override;

	uint getWidth() const                      { return m_width; }
	uint getHeight() const                     { return m_height; }
	uint getNumComponents() const              { return m_numComp; }
	EFormat getFormat() const                  { return m_format; }
	const eastl::vector<byte>& getData() const { return m_rawData; }

	inline void setPixel(uint a_x, uint a_y, const byte* a_pixelData)
	{	// Should assert arguments but slows stuff down too much
		a_x = glm::clamp(a_x, 0u, m_width - 1);
		a_y = glm::clamp(a_y, 0u, m_height - 1);
		memcpy(m_rawData.data() + ((m_width * a_y) + a_x) * m_numComp * m_pixColSize, a_pixelData, m_numComp * m_pixColSize);
		m_compressedDataUpToDate = false;
	}

	inline void getPixel(uint a_x, uint a_y, byte* a_outPixelData)
	{	// Should assert arguments but slows stuff down too much
		memcpy(a_outPixelData, m_rawData.data() + ((m_width * a_y) + a_x) * m_numComp * m_pixColSize, m_numComp * m_pixColSize);
	}

	void writeRawToCompressed();
	void writeCompressedToRaw();

	void setCompressedData(byte* data, uint64 size);

private:

	uint m_width      = 0;
	uint m_height     = 0;
	uint m_numComp    = 0;
	uint m_pixColSize = 0;
	EFormat m_format  = EFormat::BYTE;
	eastl::vector<byte> m_rawData;
	eastl::vector<byte> m_compressedData;
	bool m_compressedDataUpToDate = false;
};