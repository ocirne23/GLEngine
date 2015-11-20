#pragma once

#include "Database/IAsset.h"
#include "Utils/AtlasPosition.h"

class AtlasTexture : public IAsset
{
public:

	std::string filePath;
	int width   = 0;
	int height  = 0;
	int numComp = 0;
	AtlasPosition atlasPosition;

public:

	virtual uint getByteSize() override;
	virtual void write(std::ostream& file) override;
	virtual EAssetType getAssetType() override { return EAssetType::ATLAS_TEXTURE; }
};