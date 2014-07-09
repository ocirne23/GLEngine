#ifndef _WIN64
//Win x64 required
#endif

#include "ResourceBuilder.h"
#include "Processors\ImageProcessor.h"
#include "Processors\ModelProcessor.h"
#include "Processors\SimpleModelProcessor.h"

#include <Windows.h>

int main()
{	
	const char* IN_FILE_DIR = "C:\\Users\\Enrico\\Documents\\GitHub\\GLEngine\\GLApp\\assets-source";
	const char* OUT_FILE_DIR = "C:\\Users\\Enrico\\Documents\\GitHub\\GLEngine\\GLApp\\assets";

	ImageProcessor* imageProcessor = new ImageProcessor();
	//ModelProcessor* modelProcessor = new ModelProcessor();
	SimpleModelProcessor* modelProcessor = new SimpleModelProcessor();

	ResourceBuilder::registerProcessor("png", imageProcessor);
	ResourceBuilder::registerProcessor("tga", imageProcessor);
	ResourceBuilder::registerProcessor("bmp", imageProcessor);
	ResourceBuilder::registerProcessor("jpg", imageProcessor);
	ResourceBuilder::registerProcessor("obj", modelProcessor);

	ResourceBuilder::buildResources(IN_FILE_DIR, OUT_FILE_DIR);

	delete modelProcessor;
	delete imageProcessor;

	Sleep(5000);
	return 0;
}

//std::string da = std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(str);