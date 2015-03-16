#ifdef _WIN64 // Resource builder only works on 64 bit platforms because it requires more than 2GB memory

#include "ResourceBuilder.h"
#include "Processors\ByteImageProcessor.h"
#include "Processors\FloatImageProcessor.h"
#include "Processors\ModelProcessor.h"

#include <iostream>

enum { INCREMENTAL_BUILDING = false };

int main()
{
    ByteImageProcessor byteImageProcessor;
    FloatImageProcessor floatImageProcessor;
    ModelProcessor modelProcessor;

    ResourceBuilder::ResourceProcessorMap imageProcessors = {
	{"png", &byteImageProcessor}, {"tga", &byteImageProcessor},
	{"bmp", &byteImageProcessor}, {"jpg", &byteImageProcessor},
	{"hdr", &floatImageProcessor}
    };
    ResourceBuilder::ResourceProcessorMap modelProcessors = {
	{"obj", &modelProcessor}
    };

    ResourceBuilder::buildResources(imageProcessors, "..\\GLApp\\assets-source\\Utils", "..\\GLApp\\assets\\Utils", INCREMENTAL_BUILDING);
    ResourceBuilder::buildResources(modelProcessors, "..\\GLApp\\assets-source\\Models", "..\\GLApp\\assets\\Models", INCREMENTAL_BUILDING);

    printf("Press enter to continue.");
    std::cin.ignore();

    return 0;
}

#else // !_WIN64
#error 64 bit required
#endif //_WIN64