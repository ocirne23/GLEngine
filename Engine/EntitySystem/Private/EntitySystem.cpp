#include "EntitySystem.h"

owner<IEntityWorld*> EntitySystem::createEntityWorld()
{
	return owner<IEntityWorld*>();
}

void EntitySystem::destroyEntityWorld(owner<IEntityWorld*> entityWorld)
{
}
