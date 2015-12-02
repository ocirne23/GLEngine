#pragma once

#include "EASTL/string.h"

class AssetDatabase;

class ResourceProcessor
{
public:
	virtual ~ResourceProcessor() {}
	virtual bool process(const eastl::string& resourcePath, AssetDatabase& assetDatabase) = 0;
};