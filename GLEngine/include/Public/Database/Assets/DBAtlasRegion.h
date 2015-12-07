#pragma once

#include "Database/Assets/IAsset.h"
#include "Database/Utils/AtlasPosition.h"
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

	AtlasPosition m_atlasPosition; // position in pixels
	glm::vec4 m_atlasMapping;      // position in texture space (floats)
	int m_atlasIdx = -1;           // idx of the atlas the texture is located in in the scene.
};