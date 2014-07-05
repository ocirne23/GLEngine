#pragma once

#include "ResourceProcessor.h"

#include <unordered_map>
#include <string>

class ResourceBuilder
{
public:
	static void buildResources(const char* inDirectoryPath, const char* outDirectoryPath);
	static void registerProcessor(const char* fileExtension, ResourceProcessor* processor);

private:
	ResourceBuilder() {};

	static std::unordered_map<std::string, ResourceProcessor*> s_processors;
};