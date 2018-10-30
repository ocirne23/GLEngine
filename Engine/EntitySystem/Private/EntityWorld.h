#pragma once

class EntitySystem;

class EntityWorld
{
public:

	EntityWorld(EntitySystem& system);
	EntityWorld(const EntityWorld&) = delete;
	~EntityWorld();

private:

	EntitySystem& m_system;
};