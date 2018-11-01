#pragma once

#include "EntitySystemAPI.h"
#include <EASTL/unique_ptr.h>

class EntityWorld;
class IEntitySystem;

class ENTITYSYSTEM_API IEntityWorld
{
public:

	IEntityWorld(IEntitySystem& system);
	explicit operator EntityWorld&() { return *m_impl; }

private:
	
	eastl::unique_ptr<EntityWorld> m_impl;
};