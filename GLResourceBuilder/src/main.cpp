#ifndef _WIN64
Win x64 required
#endif

#define INCREMENTAL_BUILDING true

#include "ResourceBuilder.h"
#include "Processors\ByteImageProcessor.h"
#include "Processors\FloatImageProcessor.h"
#include "Processors\ModelProcessor.h"

#include <Windows.h>
#include <iostream>

int main()
{	
	const char* IN_FILE_DIR = "..\\GLApp\\assets-source";
	const char* OUT_FILE_DIR = "..\\GLApp\\assets";

	ByteImageProcessor byteImageProcessor;
	FloatImageProcessor floatImageProcessor;
	ModelProcessor modelProcessor;

	ResourceBuilder::registerProcessor("png", &byteImageProcessor);
	ResourceBuilder::registerProcessor("tga", &byteImageProcessor);
	ResourceBuilder::registerProcessor("bmp", &byteImageProcessor);
	ResourceBuilder::registerProcessor("jpg", &byteImageProcessor);
	ResourceBuilder::registerProcessor("hdr", &floatImageProcessor);
	ResourceBuilder::registerProcessor("obj", &modelProcessor);

	ResourceBuilder::buildResources(IN_FILE_DIR, OUT_FILE_DIR, INCREMENTAL_BUILDING);

	printf("Press enter to continue.");
	std::cin.ignore();
	return 0;
}
