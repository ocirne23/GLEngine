#ifndef _WIN64 // Resource builder only works on 64 bit platforms because it usually requires more than 2GB memory
#pragma message("Warning: GLResourceBuilder should be run in 64 bit mode")
#endif // !_WIN64

#include "ResourceBuilder.h"
#include "Database\AssetDatabase.h"
#include "Processors\ByteImageProcessor.h"
#include "Processors\FloatImageProcessor.h"
#include "Processors\SceneProcessor.h"
#include <iostream>

int main()
{
	SceneProcessor sceneProcessor;
	ResourceBuilder::ResourceProcessorMap modelProcessors;
	modelProcessors.insert({"obj", &sceneProcessor})
	
	AssetDatabase assetDatabase;
	assetDatabase.createNew("..\\GLApp\\assets\\db.da");
	ResourceBuilder::buildResourcesDB(modelProcessors, "..\\GLApp\\assets-source\\Models", assetDatabase);
	assetDatabase.writeAssetTableAndClose();

	/*
	ByteImageProcessor byteImageProcessor;
	FloatImageProcessor floatImageProcessor;
	ResourceBuilder::ResourceProcessorMap imageProcessors = {
		{"png", &byteImageProcessor}, {"tga", &byteImageProcessor},
		{"bmp", &byteImageProcessor}, {"jpg", &byteImageProcessor},
		{"hdr", &floatImageProcessor}
	};
	ResourceBuilder::buildResources(imageProcessors, "..\\GLApp\\assets-source\\UI",     "..\\GLApp\\assets\\UI",     INCREMENTAL_BUILDING);
	ResourceBuilder::buildResources(imageProcessors, "..\\GLApp\\assets-source\\Utils",  "..\\GLApp\\assets\\Utils",  INCREMENTAL_BUILDING);
	
	std::vector<std::string> copiedFileExtensions = {
		"glsl", "vert", "frag", "json"
	};
	ResourceBuilder::copyFiles(copiedFileExtensions, "..\\GLApp\\assets-source",         "..\\GLApp\\assets",         INCREMENTAL_BUILDING);
	*/

	printf("Press enter to continue.");
	std::cin.ignore();

	return 0;
}