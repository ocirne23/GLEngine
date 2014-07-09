#pragma once

#include "../ResourceProcessor.h"

class SimpleModelProcessor : public ResourceProcessor
{
public:
	virtual void process(const char* inResourcePath, const char* outResourcePath);
};