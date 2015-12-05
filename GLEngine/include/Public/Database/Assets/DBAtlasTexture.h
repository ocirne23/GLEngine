#pragma once

#include "Database/Assets/IAsset.h"
#include "Database/Assets/DBTexture.h"
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

	void writeRegionTexture(const DBAtlasRegion& region);

	uint getNumMipmaps() const { return m_numMipMaps; }
	const DBTexture& getTexture() const { return m_texture; }

public:

	uint m_numMipMaps = 0;
	DBTexture m_texture;
};