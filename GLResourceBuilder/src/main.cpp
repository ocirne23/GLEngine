#ifndef _WIN64 // Resource builder only works on 64 bit platforms because it usually requires more than 2GB memory
#pragma message("Warning: GLResourceBuilder should be ran in 64 bit mode")
#endif // !_WIN64

#include "ResourceBuilder.h"
#include "Database\AssetDatabase.h"
#include "Processors\ByteImageProcessor.h"
#include "Processors\FloatImageProcessor.h"
#include "Processors\ModelProcessor.h"
#include "Processors\SceneProcessor.h"

#include <iostream>

enum { INCREMENTAL_BUILDING = false };

int main()
{
	ByteImageProcessor byteImageProcessor;
	FloatImageProcessor floatImageProcessor;
	SceneProcessor sceneProcessor;

	ResourceBuilder::ResourceProcessorMap imageProcessors = {
		{"png", &byteImageProcessor}, {"tga", &byteImageProcessor},
		{"bmp", &byteImageProcessor}, {"jpg", &byteImageProcessor},
		{"hdr", &floatImageProcessor}
	};
	ResourceBuilder::ResourceProcessorMap modelProcessors = {
		{ "obj", &sceneProcessor },
		//{"ifc", &sceneProcessor}
		//{"rvt", &sceneProcessor}
	};
	ResourceBuilder::ResourceProcessorMap sceneProcessors = {
		{"ifc", &sceneProcessor}
	};
	std::vector<std::string> copiedFileExtensions = {"glsl", "vert", "frag", "json"};
	
	ResourceBuilder::buildResources(modelProcessors, "..\\GLApp\\assets-source\\Models", "..\\GLApp\\assets\\Models", INCREMENTAL_BUILDING);
	ResourceBuilder::buildResources(sceneProcessors, "..\\GLApp\\assets-source\\Models", "..\\GLApp\\assets\\Models", INCREMENTAL_BUILDING);

	AssetDatabase assetDatabase;
	assetDatabase.create("..\\GLApp\\assets\\db.da");
	ResourceBuilder::buildResourcesDB(modelProcessors, "..\\GLApp\\assets-source\\Models", assetDatabase, INCREMENTAL_BUILDING);
	assetDatabase.flush();
	assetDatabase.saveAndClose();

	/*
	ResourceBuilder::buildResources(imageProcessors, "..\\GLApp\\assets-source\\UI",     "..\\GLApp\\assets\\UI",     INCREMENTAL_BUILDING);
	ResourceBuilder::buildResources(imageProcessors, "..\\GLApp\\assets-source\\Utils",  "..\\GLApp\\assets\\Utils",  INCREMENTAL_BUILDING);
	ResourceBuilder::buildResources(modelProcessors, "..\\GLApp\\assets-source\\Models", "..\\GLApp\\assets\\Models", INCREMENTAL_BUILDING);
	ResourceBuilder::copyFiles(copiedFileExtensions, "..\\GLApp\\assets-source",         "..\\GLApp\\assets",         INCREMENTAL_BUILDING);
	*/
	//printf("Press enter to continue.");
	std::cin.ignore();

	return 0;
}