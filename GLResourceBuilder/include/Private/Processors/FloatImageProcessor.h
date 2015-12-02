#pragma once

#include "Processors/ResourceProcessor.h"

class FloatImageProcessor : public ResourceProcessor
{
public:
	virtual bool process(const eastl::string& resourcePath, AssetDatabase& assetDatabase) override;
};