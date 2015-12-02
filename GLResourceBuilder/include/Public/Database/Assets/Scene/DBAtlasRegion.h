#pragma once

#include "Database/IAsset.h"
#include "Utils/AtlasPosition.h"
#include "EASTL/string.h"

class DBAtlasRegion : public IAsset
{
public:

	DBAtlasRegion() {}
	virtual ~DBAtlasRegion() {}

	void loadInfo(const eastl::string& filePath);
	virtual uint64 getByteSize() const override;
	virtual EAssetType getAssetType() const override { return EAssetType::ATLAS_REGION; }
	virtual void write(AssetDatabaseEntry& entry) override;
	virtual void read(AssetDatabaseEntry& entry) override;

public:

	eastl::string m_filePath;
	uint m_width = 0;
	uint m_height = 0;
	uint m_numComp = 0;
	AtlasPosition m_atlasPosition;
};