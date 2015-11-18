#pragma once

#include "Database/IAsset.h"

class Texture : public IAsset
{
public:

	virtual uint getByteSize() override;
	virtual void write(std::ostream& file) override;
	virtual EAssetType getAssetType() override { return EAssetType::TEXTURE; }
};