#pragma once

#include "Core.h"
#include "EntitySystemAPI.h"

class IEntitySystem;

class ENTITYSYSTEM_API EntitySystemModule
{
public:

	static owner<IEntitySystem*> createEntitySystem();
	static void destroyEntitySystem(owner<IEntitySystem*> entitySystem);

private:

	EntitySystemModule() {}
	~EntitySystemModule() {}
};