#include "EntitySystem.h"

owner<IEntitySystem*> IEntitySystem::create()
{
	return new EntitySystem();
}