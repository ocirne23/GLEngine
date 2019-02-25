#pragma once

#include "Core.h"
#include "EntitySystemAPI.h"

class EntitySystem;

class ENTITYSYSTEM_API IEntitySystem
{
public:

	static owner<IEntitySystem*> create();
};