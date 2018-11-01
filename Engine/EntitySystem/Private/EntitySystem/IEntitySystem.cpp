#include "EntitySystem/IEntitySystem.h"

#include "EntitySystem.h"

IEntitySystem::IEntitySystem()
	: m_impl(new EntitySystem())
{
}

void IEntitySystem::doStuff() 
{ 
	m_impl->doStuff();
}