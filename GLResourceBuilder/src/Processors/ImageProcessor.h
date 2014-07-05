#pragma once

#include "../ResourceProcessor.h"

class ImageProcessor : public ResourceProcessor
{
public:
	virtual void process(const char* inResourcePath, const char* outResourcePath);
};