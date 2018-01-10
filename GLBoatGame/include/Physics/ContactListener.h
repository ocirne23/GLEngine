#pragma once

#include "Core.h"

#include <Box2D/Box2D.h>

struct ContactMessage
{
	uint entityA;
	uint entityB;
	uint fixtureAIndex;
	uint fixtureBIndex;
	b2Vec2 contactPoint;
};

class PhysicsSystem;
class LogicSystem;

class ContactListener : public b2ContactListener
{
public:
	ContactListener(PhysicsSystem& a_physicsSystem, LogicSystem& a_logicSystem)
		: m_physicsSystem(a_physicsSystem)
		, m_logicSystem(a_logicSystem)
	{
	}

	virtual ~ContactListener() {}

	virtual void BeginContact(b2Contact* contact) override;
	virtual void EndContact(b2Contact* contact) override;
	virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override;
	virtual void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override;

private:

	LogicSystem& m_logicSystem;
	PhysicsSystem& m_physicsSystem;
};
