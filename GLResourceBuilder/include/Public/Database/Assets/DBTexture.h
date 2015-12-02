#pragma once

#include "Database/IAsset.h"

class DBTexture : public IAsset
{
public:

	DBTexture() {}
	DBTexture(const rde::string& sourceFilePath);
	virtual ~DBTexture() {}

	virtual uint64 getByteSize() const override;
	virtual EAssetType getAssetType() const override { return EAssetType::TEXTURE; }
	virtual void write(AssetDatabaseEntry& entry) override;
	virtual void read(AssetDatabaseEntry& entry) override;

private:

	rde::string m_filePath;
	int m_width         = 0;
	int m_height        = 0;
	int m_numComp       = 0;
	bool m_isFloatImage = false;
	rde::vector<byte> m_data;
};