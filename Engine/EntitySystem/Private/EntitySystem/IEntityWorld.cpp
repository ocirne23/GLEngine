#include "EntitySystem/IEntityWorld.h"

#include "EntityWorld.h"
#include "EntitySystem/IEntitySystem.h"
#include "EntitySystem.h"

IEntityWorld::IEntityWorld(IEntitySystem& system)
	: m_impl(new EntityWorld(scast<EntitySystem&>(system)))
{
}