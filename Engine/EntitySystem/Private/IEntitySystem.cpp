#include "EntitySystem/IEntitySystem.h"

#include "EntitySystem.h"

IEntitySystem::IEntitySystem()
	: m_impl(make_pimpl<EntitySystem>())
{
}

void IEntitySystem::doStuff() 
{ 
	m_impl->doStuff();
}