#pragma once

#include "Core.h"

class IEntityWorld;

class EntitySystem
{
public:
	owner<IEntityWorld*> createEntityWorld();
	void destroyEntityWorld(owner<IEntityWorld*> entityWorld);

};