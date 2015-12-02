#pragma once

#include "Database/IAsset.h"

class DBShader : public IAsset
{
public:

	DBShader() {}
	virtual ~DBShader() {}

	virtual uint64 getByteSize() const override { return 0; }
	virtual EAssetType getAssetType() const override { return EAssetType::SHADER; }
	virtual void write(AssetDatabaseEntry& entry) override {}
	virtual void read(AssetDatabaseEntry& entry) override {}
};