#pragma once

#include "Core.h"
#include "ECS/Entity.h"
#include "Utils/Mutex.h"
#include <EASTL/bonus/ring_buffer.h>

class EntitySystem
{
public:

	EntitySystem() : m_freeIDs(8) {}

	Entity createEntity();
	void deleteEntity(Entity& e);

private:

	uint m_numUsedIDs = 1;
	eastl::ring_buffer<Entity> m_freeIDs;
	Mutex m_mutex;
};