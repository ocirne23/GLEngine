#pragma once

#include "Processors/ResourceProcessor.h"

#include <unordered_map>
#include <string>

class ResourceBuilder
{
public:
	typedef std::unordered_map<std::string, ResourceProcessor*> ResourceProcessorMap;
	static void buildResources(const std::unordered_map<std::string, ResourceProcessor*>& processors, const char* inDirectoryPath, const char* outDirectoryPath, bool incremental);
	static void copyFiles(const std::vector<std::string>& extensions, const char* inDirectoryPath, const char* outDirectoryPath, bool incremental);

private:
	ResourceBuilder() {};
};