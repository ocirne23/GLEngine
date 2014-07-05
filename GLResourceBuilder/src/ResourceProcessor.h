#pragma once

class ResourceProcessor
{
public:
	virtual ~ResourceProcessor() {}

	virtual void process(const char* inResourcePath, const char* outResourcePath) = 0;
};