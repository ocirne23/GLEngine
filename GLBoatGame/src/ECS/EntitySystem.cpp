#include "ECS/EntitySystem.h"

#include "Utils/ScopeLock.h"

Entity EntitySystem::createEntity()
{
	ScopeLock l(m_mutex);
	if (!m_freeIDs.empty())
	{
		Entity entity = m_freeIDs.front();
		m_freeIDs.pop_front();
		return entity;
	}
	else
	{
		Entity e(m_numUsedIDs++, 1);
		assert(m_numUsedIDs < Entity::MAX_NUM_ENTITIES);
		return e;
	}
}

void EntitySystem::deleteEntity(Entity& a_entity)
{
	ScopeLock l(m_mutex);
	a_entity.version++;
	if (m_freeIDs.size() == m_freeIDs.capacity())
		m_freeIDs.set_capacity(m_freeIDs.capacity() * 2);
	m_freeIDs.push_back(a_entity);
}
