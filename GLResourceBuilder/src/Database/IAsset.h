#pragma once

#include "BuilderCore.h"

#include "Database/EAssetType.h"

#include <ostream>

class IAsset
{
public:
	IAsset() {}
	virtual ~IAsset() {}

	virtual uint getByteSize() = 0;
	virtual EAssetType getAssetType() = 0;
	virtual void write(std::ostream& file) = 0;

private:

};