#ifndef _WIN64
Win x64 required
#endif

#define INCREMENTAL_BUILDING false

#include "ResourceBuilder.h"
#include "Processors\ImageProcessor.h"
#include "Processors\ModelProcessor.h"

#include <Windows.h>

int main()
{	
	const char* IN_FILE_DIR = "..\\GLApp\\assets-source";
	const char* OUT_FILE_DIR = "..\\GLApp\\assets";

	ImageProcessor* imageProcessor = new ImageProcessor();
	ModelProcessor* modelProcessor = new ModelProcessor();

	ResourceBuilder::registerProcessor("png", imageProcessor);
	ResourceBuilder::registerProcessor("tga", imageProcessor);
	ResourceBuilder::registerProcessor("bmp", imageProcessor);
	ResourceBuilder::registerProcessor("jpg", imageProcessor);
	ResourceBuilder::registerProcessor("obj", modelProcessor);

	ResourceBuilder::buildResources(IN_FILE_DIR, OUT_FILE_DIR, INCREMENTAL_BUILDING);

	delete modelProcessor;
	delete imageProcessor;

	Sleep(5000);
	return 0;
}
