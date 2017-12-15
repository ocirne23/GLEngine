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

struct Entity
{
	static constexpr EntityIndex MAX_NUM_ENTITIES = ((uint64)1 << INDEX_BITS) - (uint64)1;

	Entity() : index(0), version(0) {}
	Entity(EntityIndex a_index, EntityVersion a_version) 
		: index(a_index)
		, version(a_version)
	{
	}
	uint getID() const { return *(uint*) this; }

	EntityIndex index;
	EntityVersion version;
};