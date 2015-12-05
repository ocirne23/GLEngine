#pragma once

#include "Database/AssetDatabase.h"
#include "EASTL/string.h"

class ResourceProcessor
{
public:
	virtual ~ResourceProcessor() {}
	virtual bool process(const eastl::string& resourcePath, AssetDatabase& assetDatabase) = 0;
};