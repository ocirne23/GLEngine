#pragma once

#include "Database/Assets/IAsset.h"
#include "EASTL/string.h"

#include <glm/glm.hpp>

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

	// Base texture info (not the atlas)
	eastl::string m_filePath;
	uint m_texWidth  = 0;
	uint m_texHeight = 0;
	uint m_numComp   = 0;

	glm::uvec4 m_atlasPosition; // position and size in pixels inside the atlas
	glm::vec4 m_atlasMapping;   // uv coordionates of the region inside the atlas
	int m_atlasIdx = -1;        // array slice of the atlas array the region is in.
};