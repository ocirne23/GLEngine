#pragma once

#include "Processors/ResourceProcessor.h"

class ByteImageProcessor : public ResourceProcessor
{
public:
	virtual bool process(const std::string& resourcePath, AssetDatabase& assetDatabase) override;
};