#pragma once

#include "EntitySystemAPI.h"

class EntityWorld;
class IEntitySystem;

class ENTITYSYSTEM_API IEntityWorld
{
public:

	IEntityWorld(IEntitySystem& system);
	IEntityWorld(const IEntityWorld&) = delete;
	~IEntityWorld();
	explicit operator EntityWorld&() { return *m_impl; }

private:
	
	EntityWorld* m_impl;
};