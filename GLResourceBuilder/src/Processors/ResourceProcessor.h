#pragma once

#include <string>

class AssetDatabase;

class ResourceProcessor
{
public:
	virtual ~ResourceProcessor() {}
	virtual bool process(const std::string& resourcePath, AssetDatabase& assetDatabase) = 0;
};