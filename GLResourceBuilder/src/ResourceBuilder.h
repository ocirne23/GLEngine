#pragma once

#include "Processors/ResourceProcessor.h"
#include <unordered_map>
#include <string>

class AssetDatabase;

class ResourceBuilder
{
public:
	typedef std::unordered_map<std::string, ResourceProcessor*> ResourceProcessorMap;
	static void buildResourcesDB(const std::unordered_map<std::string, ResourceProcessor*>& processors, const char* inDirectoryPath, AssetDatabase& assetDatabase, bool incremental);
	static void copyFiles(const std::vector<std::string>& extensions, const char* inDirectoryPath, const char* outDirectoryPath, bool incremental);
private:
	ResourceBuilder() {};
};