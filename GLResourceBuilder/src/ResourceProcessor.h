#pragma once

enum ResourceType
{
	ResourceType_MODEL,
	ResourceType_BYTEIMAGE,
	ResourceType_FLOATIMAGE
};

class ResourceProcessor
{
public:
	virtual ~ResourceProcessor() {}

	virtual bool process(const char* inResourcePath, const char* outResourcePath) = 0;
};