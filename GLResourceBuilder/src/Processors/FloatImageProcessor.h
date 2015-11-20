#pragma once

#include "Processors/ResourceProcessor.h"

class FloatImageProcessor : public ResourceProcessor
{
public:
	virtual bool process(const char* inResourcePath, const char* outResourcePath, std::vector<std::string>& rebuildDependencies) override;
	virtual void process(const char* inResourcePath, AssetDatabase& assetDatabase, std::vector<std::string>& rebuildOnFileModificationList) override;
};