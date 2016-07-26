#include "GLEngine.h"

#include "Database/AssetDatabase.h"
#include "Database/Processors/SceneProcessor.h"
#include "Database/ResourceBuilder.h"

#include <iostream>

int main()
{
	GLEngine::initialize("GLResourceBuilder", 0, 0, EWindowMode::NONE);

	SceneProcessor sceneProcessor;
	AssetDatabase objDB;

	ResourceBuilder::ResourceProcessorMap processors = {{"obj", &sceneProcessor}};
	objDB.createNew("..\\GLApp\\assets\\OBJ-DB.da");
	ResourceBuilder::buildResourcesDB(processors, "..\\GLApp\\assets\\Models", objDB);
	objDB.writeAndClose();

	print("Press enter to exit\n");
	std::cin.ignore();

	GLEngine::finish();
	return 0;
}