#pragma once

#include "Core.h"

class PhysicsSystem;
class LogicSystem;
class RenderSystem;

class BoatGame
{
public:

	BoatGame();
	~BoatGame();

	PhysicsSystem& getPhysicsSystem() { return *m_physicsSystem; }
	LogicSystem& getLogicSystem()     { return *m_logicSystem; }
	RenderSystem& getRenderSystem()   { return *m_renderSystem; }

	const PhysicsSystem& getPhysicsSystem() const { return *m_physicsSystem; }
	const LogicSystem& getLogicSystem() const     { return *m_logicSystem; }
	const RenderSystem& getRenderSystem() const   { return *m_renderSystem; }

private:

	owner<PhysicsSystem*> m_physicsSystem = NULL;
	owner<LogicSystem*> m_logicSystem     = NULL;
	owner<RenderSystem*> m_renderSystem   = NULL;
};