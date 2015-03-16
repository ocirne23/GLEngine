#pragma once

class ResourceProcessor
{
public:
    virtual ~ResourceProcessor() {}

    virtual bool process(const char* inResourcePath, const char* outResourcePath) = 0;
};