#include "GLEngine.h"

#include "Database/AssetDatabase.h"
#include "Database/Processors/SceneProcessor.h"
#include "Database/ResourceBuilder.h"

#include <iostream>

int main()
{
	GLEngine::initialize("GLResourceBuilder", 0, 0, false);

	SceneProcessor sceneProcessor;
	
	ResourceBuilder::ResourceProcessorMap ifcProcessors;
	ifcProcessors.insert({"ifc", &sceneProcessor});
	AssetDatabase ifcDB;
	ifcDB.createNew("..\\GLApp\\assets\\IFC-DB.da");
	ResourceBuilder::buildResourcesDB(ifcProcessors, "..\\GLApp\\assets-source\\Models", ifcDB);
	ifcDB.writeAssetTableAndClose();
	
	ResourceBuilder::ResourceProcessorMap objProcessors;
	objProcessors.insert({"obj", &sceneProcessor});
	AssetDatabase objDB;
	objDB.createNew("..\\GLApp\\assets\\OBJ-DB.da");
	ResourceBuilder::buildResourcesDB(objProcessors, "..\\GLApp\\assets-source\\Models", objDB);
	objDB.writeAssetTableAndClose();

	print("Press enter to exit\n");
	std::cin.ignore();

	GLEngine::finish();
	return 0;
}