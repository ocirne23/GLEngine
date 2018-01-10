#include "Physics/ContactListener.h"
#include "ECS/PhysicsSystem.h"
#include "ECS/LogicSystem.h"

BEGIN_UNNAMED_NAMESPACE()

uint userdataToUint(void* userData)
{
	return scast<uint>(rcast<size_t>(userData));
}

END_UNNAMED_NAMESPACE()

void ContactListener::BeginContact(b2Contact* a_contact)
{
	const b2Fixture* a = a_contact->GetFixtureA();
	const b2Fixture* b = a_contact->GetFixtureB();

	ContactMessage m;
	m.fixtureAIndex = userdataToUint(a->GetUserData());
	m.fixtureBIndex = userdataToUint(a->GetUserData());
	m.entityA = userdataToUint(a->GetBody()->GetUserData());
	m.entityB = userdataToUint(b->GetBody()->GetUserData());
	m.contactPoint = a_contact->GetManifold()->points[0].localPoint;
	m_logicSystem.addBeginContactMessage(m);
}

void ContactListener::EndContact(b2Contact* a_contact)
{
	const b2Fixture* a = a_contact->GetFixtureA();
	const b2Fixture* b = a_contact->GetFixtureB();

	ContactMessage m;
	m.fixtureAIndex = userdataToUint(a->GetUserData());
	m.fixtureBIndex = userdataToUint(a->GetUserData());
	m.entityA = userdataToUint(a->GetBody()->GetUserData());
	m.entityB = userdataToUint(b->GetBody()->GetUserData());
	m.contactPoint = a_contact->GetManifold()->points[0].localPoint;
	m_logicSystem.addEndContactMessage(m);
}

void ContactListener::PreSolve(b2Contact* a_contact, const b2Manifold* a_oldManifold)
{
}

void ContactListener::PostSolve(b2Contact* a_contact, const b2ContactImpulse* a_impulse)
{
}
