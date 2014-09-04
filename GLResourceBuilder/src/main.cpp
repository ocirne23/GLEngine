#ifndef _WIN64
Win x64 required
#endif

#define INCREMENTAL_BUILDING true

#include "ResourceBuilder.h"
#include "Processors\ByteImageProcessor.h"
#include "Processors\FloatImageProcessor.h"
#include "Processors\ModelProcessor.h"

#include <iostream>

int main()
{	
	ByteImageProcessor byteImageProcessor;
	FloatImageProcessor floatImageProcessor;
	ModelProcessor modelProcessor;

	ResourceBuilder::ResourceProcessorMap imageProcessors = {
		{ "png", &byteImageProcessor }, { "tga", &byteImageProcessor },
		{ "bmp", &byteImageProcessor }, { "jpg", &byteImageProcessor }, 
		{ "hdr", &floatImageProcessor }
	};
	ResourceBuilder::ResourceProcessorMap modelProcessors = {
		{ "obj", &modelProcessor }
	};

	ResourceBuilder::buildResources(imageProcessors, "..\\GLApp\\assets-source\\Utils", "..\\GLApp\\assets\\Utils", INCREMENTAL_BUILDING);
	ResourceBuilder::buildResources(modelProcessors, "..\\GLApp\\assets-source\\Models", "..\\GLApp\\assets\\Models", INCREMENTAL_BUILDING);

	printf("Press enter to continue.");
	std::cin.ignore();

	return 0;
}
