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
	/*
	ResourceBuilder::ResourceProcessorMap ifcProcessors;
	ifcProcessors.insert({"ifc", &sceneProcessor});
	AssetDatabase ifcDB;
	ifcDB.createNew("..\\GLApp\\assets\\IFC-DB.da");
	ResourceBuilder::buildResourcesDB(ifcProcessors, "..\\GLApp\\assets-source\\Models", ifcDB);
	ifcDB.writeAndClose();
	*/
	ResourceBuilder::ResourceProcessorMap objProcessors;
	objProcessors.insert({"obj", &sceneProcessor});

	objDB.createNew("..\\GLApp\\assets\\OBJ-DB.da");
	ResourceBuilder::buildResourcesDB(objProcessors, "..\\GLApp\\assets\\Models", objDB);
	objDB.writeAndClose();

	print("Press enter to exit\n");
	std::cin.ignore();

	GLEngine::finish();
	return 0;
}