#pragma once

#include <string>
#include <vector>

class ResourceProcessor
{
public:
	virtual ~ResourceProcessor() {}

	virtual bool process(const char* inResourcePath, const char* outResourcePath, std::vector<std::string>& rebuildOnFileModificationList) = 0;
};