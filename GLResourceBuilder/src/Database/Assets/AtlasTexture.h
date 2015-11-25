#pragma once

#include "Database/IAsset.h"
#include "Utils/AtlasPosition.h"

class AtlasTexture : public IAsset
{
public:

	AtlasTexture(const std::string& databaseEntryName);

	virtual uint getByteSize() const override;
	virtual EAssetType getAssetType() const override { return EAssetType::ATLAS_TEXTURE; }
	virtual void write(std::ostream& file) override;
	virtual void read(std::istream& file) override;

private:

	std::string m_filePath;
	int m_width = 0;
	int m_height = 0;
	int m_numComp = 0;
	AtlasPosition m_atlasPosition;
};