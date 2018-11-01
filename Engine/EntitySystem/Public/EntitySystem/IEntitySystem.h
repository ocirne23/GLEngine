#pragma once

#include "EntitySystemAPI.h"
#include <EASTL/unique_ptr.h>

class EntitySystem;

class ENTITYSYSTEM_API IEntitySystem
{
public:

	IEntitySystem();
	explicit operator EntitySystem&() { return *m_impl; }

	void doStuff();

private:

	eastl::unique_ptr<EntitySystem> m_impl;
};