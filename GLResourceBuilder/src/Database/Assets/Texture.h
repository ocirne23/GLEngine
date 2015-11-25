#pragma once

#include "Database/IAsset.h"
#include <string>

class Texture : public IAsset
{
public:

	Texture(const std::string& sourceFilePath);
	~Texture() {}

	virtual uint getByteSize() override;
	virtual void write(std::ostream& file) override;
	virtual EAssetType getAssetType() override { return EAssetType::TEXTURE; }

public:

	std::string m_filePath;
	int m_width         = 0;
	int m_height        = 0;
	int m_numComp       = 0;
	bool m_isFloatImage = false;
};