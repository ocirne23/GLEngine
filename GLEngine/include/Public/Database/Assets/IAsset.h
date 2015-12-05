#pragma once

#include "Core.h"
#include "Database/Assets/EAssetType.h"
#include "Database/AssetDatabaseEntry.h"

class IAsset
{
public:

	virtual ~IAsset() {}

	virtual uint64 getByteSize() const            = 0;
	virtual EAssetType getAssetType() const       = 0;
	virtual void write(AssetDatabaseEntry& entry) = 0;
	virtual void read(AssetDatabaseEntry& entry)  = 0;
	
public:

	static IAsset* create(EAssetType type);
};