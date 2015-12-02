#pragma once

#include "Database/IAsset.h"
#include "EASTL/vector.h"

class DBAtlasRegion;

class DBAtlasTexture : public IAsset
{
public:

	DBAtlasTexture() {}
	DBAtlasTexture(uint width, uint height, uint numComponents, uint numMipMaps);
	virtual ~DBAtlasTexture() {}

	virtual uint64 getByteSize() const override;
	virtual EAssetType getAssetType() const override { return EAssetType::ATLAS_TEXTURE; }
	virtual void write(AssetDatabaseEntry& entry) override;
	virtual void read(AssetDatabaseEntry& entry) override;

	void setRegion(const DBAtlasRegion& region);

public:

	uint m_width             = 0;
	uint m_height            = 0;
	uint m_numComponents     = 0;
	uint m_numMipMaps        = 0;
	eastl::vector<byte> m_atlasTextureData;
};