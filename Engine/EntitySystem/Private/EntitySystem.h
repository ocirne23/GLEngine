#pragma once

#include "Core.h"

class EntityWorld;

class EntitySystem
{
public:

	EntitySystem();
	EntitySystem(const EntitySystem&) = delete;
	~EntitySystem();

	void doStuff();
};