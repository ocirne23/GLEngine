#pragma once

#include "ResourceProcessor.h"

class FloatImageProcessor : public ResourceProcessor
{
public:
	virtual bool process(const char* inResourcePath, const char* outResourcePath);
};