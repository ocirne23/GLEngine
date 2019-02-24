#pragma once

#include "EntitySystemAPI.h"
#include "Core/Pimpl.h"

class EntityWorld;
class IEntitySystem;

class ENTITYSYSTEM_API IEntityWorld
{
public:

	IEntityWorld(IEntitySystem& system);
	explicit operator EntityWorld&() { return *m_impl; }

private:
	
	Pimpl<EntityWorld> m_impl;
};