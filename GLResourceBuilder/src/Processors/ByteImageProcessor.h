#pragma once

#include "Processors/ResourceProcessor.h"

class ByteImageProcessor : public ResourceProcessor
{
public:
	virtual bool process(const char* inResourcePath, const char* outResourcePath, std::vector<std::string>& rebuildOnFileModificationList);
};