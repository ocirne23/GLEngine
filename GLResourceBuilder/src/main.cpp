#include "GLEngine.h"

#include "Database/AssetDatabase.h"
#include "Database/Processors/SceneProcessor.h"
#include "Database/ResourceBuilder.h"

#include <iostream>

int main()
{
	GLEngine::initialize();

	SceneProcessor sceneProcessor;
	ResourceBuilder::ResourceProcessorMap processors;
	processors.insert({"obj", &sceneProcessor});

	AssetDatabase db;
	db.createNew("..\\GLApp\\assets\\db.da");
	ResourceBuilder::buildResourcesDB(processors, "..\\GLApp\\assets-source\\Models", db);
	db.writeAssetTableAndClose();
	print("Press enter to exit\n");
	std::cin.ignore();

	GLEngine::finish();
	return 0;
}