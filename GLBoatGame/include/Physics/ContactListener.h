#pragma once

#include <Box2D/Box2D.h>

class PhysicsSystem;

class ContactListener : public b2ContactListener
{
public:
	ContactListener(PhysicsSystem& a_physicsSystem)
		: m_physicsSystem(a_physicsSystem)
	{
	}

	virtual ~ContactListener() {}

	virtual void BeginContact(b2Contact* a_contact) override;
	virtual void EndContact(b2Contact* contact) override;
	virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override;
	virtual void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override;

private:

	PhysicsSystem& m_physicsSystem;
};
