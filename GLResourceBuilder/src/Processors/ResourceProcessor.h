#pragma once

#include <string>
#include <vector>

class AssetDatabase;

class ResourceProcessor
{
public:
	virtual ~ResourceProcessor() {}

	virtual bool process(const char* inResourcePath, const char* outResourcePath, std::vector<std::string>& rebuildOnFileModificationList) = 0;
	virtual void process(const char* inResourcePath, AssetDatabase& assetDatabase, std::vector<std::string>& rebuildOnFileModificationList) = 0;
};