#pragma once

class ResourceGenerator
{
public:
	virtual ~ResourceGenerator() {}

	virtual bool generate(const char* outResourcePath) = 0;
};