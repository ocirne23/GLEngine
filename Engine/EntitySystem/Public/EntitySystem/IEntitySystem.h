#pragma once

#include "EntitySystemAPI.h"

class EntitySystem;

class ENTITYSYSTEM_API IEntitySystem
{
public:

	IEntitySystem();
	IEntitySystem(const IEntitySystem&) = delete;
	~IEntitySystem();
	explicit operator EntitySystem&() { return *m_impl; }

	void doStuff();

private:

	EntitySystem* m_impl = nullptr;
};