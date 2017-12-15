#pragma once

#include "Core.h"

class PhysicsSystem;
class LogicSystem;
class RenderSystem;
class EntitySystem;

class BoatGame
{
public:

	BoatGame();
	~BoatGame();

	PhysicsSystem& getPhysicsSystem() { return *m_physicsSystem; }
	LogicSystem& getLogicSystem()     { return *m_logicSystem; }
	RenderSystem& getRenderSystem()   { return *m_renderSystem; }
	EntitySystem& getEntitySystem()   { return *m_entitySystem; }

	const PhysicsSystem& getPhysicsSystem() const { return *m_physicsSystem; }
	const LogicSystem& getLogicSystem() const     { return *m_logicSystem; }
	const RenderSystem& getRenderSystem() const   { return *m_renderSystem; }
	const EntitySystem& getEntitySystem() const   { return *m_entitySystem; }

private:

	owner<PhysicsSystem*> m_physicsSystem = NULL;
	owner<LogicSystem*> m_logicSystem     = NULL;
	owner<RenderSystem*> m_renderSystem   = NULL;
	owner<EntitySystem*> m_entitySystem   = NULL;
};