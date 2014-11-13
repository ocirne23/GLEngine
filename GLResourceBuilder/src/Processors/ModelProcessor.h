#pragma once

#include "ResourceProcessor.h"

class ModelProcessor : public ResourceProcessor
{
public:
	virtual bool process(const char* inResourcePath, const char* outResourcePath);
};