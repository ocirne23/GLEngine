#include "EntitySystemModule.h"

owner<IEntitySystem*> EntitySystemModule::createEntitySystem()
{
	return owner<IEntitySystem*>();
}

void EntitySystemModule::destroyEntitySystem(owner<IEntitySystem*> entitySystem)
{
}
