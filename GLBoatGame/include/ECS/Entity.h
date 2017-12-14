#pragma once

#include "Core.h"

typedef uint EntityIDType;
typedef ushort EntityIndex;
typedef ushort EntityVersion;
class World;

BEGIN_UNNAMED_NAMESPACE()

constexpr uint INDEX_BITS = sizeof(EntityIndex) * 8;
constexpr uint VERSION_BITS = sizeof(EntityVersion) * 8;
constexpr uint64 INDEX_MASK = ((uint64) 1 << INDEX_BITS) - (uint64) 1;

static_assert(INDEX_BITS + VERSION_BITS == sizeof(EntityIDType) * 8, "Mismatch bits and EntityID");

END_UNNAMED_NAMESPACE()

struct EntityID
{
	EntityID() {}
	EntityID(EntityIndex a_index, EntityVersion a_version) 
		: index(a_index)
		, version(a_version)
	{
	}
	uint getID() const { return *(uint*) this; }

	uint index : INDEX_BITS;
	uint version : VERSION_BITS;
};

class Entity
{
public:

	static constexpr EntityIndex MAX_NUM_ENTITIES = ((uint64) 1 << INDEX_BITS) - (uint64)1;

	Entity(not_null<World*> a_world, EntityIndex a_index, EntityVersion a_version) 
		: m_world(a_world)
		, m_id({ a_index, a_version })
	{
	}

	const World* getWorld() const
	{
		return m_world;
	}

	World* getWorld()
	{
		return m_world;
	}

	uint getID() const { return m_id.getID(); }

public:

	const EntityID m_id;

private:

	World* m_world = NULL;
};