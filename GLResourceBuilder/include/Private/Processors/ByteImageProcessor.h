#pragma once

#include "Processors/ResourceProcessor.h"

class ByteImageProcessor : public ResourceProcessor
{
public:
	virtual bool process(const eastl::string& resourcePath, AssetDatabase& assetDatabase) override;
};