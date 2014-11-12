#pragma once

#include "../ResourceProcessor.h"

class GW2SongProcessor : public ResourceProcessor
{
public:
	virtual bool process(const char* inResourcePath, const char* outResourcePath);
};