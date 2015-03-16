#pragma once

#include "../ResourceProcessor.h"

class ByteImageProcessor : public ResourceProcessor
{
public:
    virtual bool process(const char* inResourcePath, const char* outResourcePath);
};