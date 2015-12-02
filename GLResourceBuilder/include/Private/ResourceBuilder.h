#pragma once

#include "Processors/ResourceProcessor.h"
#include "EASTL/string.h"
#include "EASTL/vector.h"
#include "EASTL/hash_map.h"

class AssetDatabase;

class ResourceBuilder
{
public:
	typedef eastl::hash_map<eastl::string, ResourceProcessor*> ResourceProcessorMap;
	static void buildResourcesDB(const ResourceProcessorMap& processors, const eastl::string& inDirectoryPath, AssetDatabase& assetDatabase);
	static void copyFiles(const eastl::vector<eastl::string>& extensions, const eastl::string& inDirectoryPath, const eastl::string& outDirectoryPath);
private:
	ResourceBuilder() {};
};