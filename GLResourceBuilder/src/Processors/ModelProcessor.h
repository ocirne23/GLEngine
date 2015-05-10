#pragma once

#include "Processors/ResourceProcessor.h"

class ModelProcessor : public ResourceProcessor
{
public:
	virtual bool process(const char* inResourcePath, const char* outResourcePath, std::vector<std::string>& rebuildDependencies);
};