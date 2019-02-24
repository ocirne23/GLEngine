#pragma once

#include "EntitySystemAPI.h"
#include "Core/Pimpl.h"

class EntitySystem;

class ENTITYSYSTEM_API IEntitySystem
{
public:

	IEntitySystem();
	explicit operator EntitySystem&() { return *m_impl; }

	void doStuff();

private:

	Pimpl<EntitySystem> m_impl;
};