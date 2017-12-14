#include "ECS/PhysicsSystem.h"

#include "Physics/ContactListener.h"
#include "Graphics/Utils/Box2DDebugDraw.h"

#include <assert.h>
#include <Box2D/Box2D.h>

BEGIN_UNNAMED_NAMESPACE()

const float TIMESTEP = 1 / 20.0f;
const int POSITION_ITERATIONS = 3;
const int VELOCITY_ITERATIONS = 8;

END_UNNAMED_NAMESPACE()

PhysicsSystem::PhysicsSystem(BoatGame& a_boatGame)
	: m_boatGame(a_boatGame)
{
	m_contactListener = new ContactListener(*this);
	m_physicsWorld = new b2World(b2Vec2(0.0f, 0.0f));

	m_physicsWorld->SetContactListener(m_contactListener);

	b2BodyDef bodyDef;
	b2Body* body = m_physicsWorld->CreateBody(&bodyDef);
	b2CircleShape shape;
	shape.m_radius = 3.0f;
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &shape;
	b2Fixture* fixture = body->CreateFixture(&fixtureDef);
	body->SetActive(true);
}

PhysicsSystem::~PhysicsSystem()
{
	SAFE_DELETE(m_contactListener);
	SAFE_DELETE(m_debugDraw);
	SAFE_DELETE(m_physicsWorld);
}

void PhysicsSystem::update(float a_deltaSec)
{
	m_messageQueueSemaphore.acquire();
	{
		for (CreateBodyMessage& message : m_createBodyMessageQueue)
		{
			assert(m_components[message.id.index].body == NULL);
			message.bodyDef.userData = rcast<void*>(scast<uint64>(message.id.index));
			b2Body* body = m_physicsWorld->CreateBody(&message.bodyDef);
			for (auto& fixtureDef : message.fixtureDefs)
			{
				b2Fixture* fixture = body->CreateFixture(&fixtureDef);
			}
			m_components[message.id.index].body = body;
		}
		m_createBodyMessageQueue.clear();

		for (CreateJointMessage& message : m_createJointMessageQueue)
		{
			assert(m_components[message.id.index].body != NULL);
			b2Joint* joint = m_physicsWorld->CreateJoint(message.jointDef.get());
		}
		m_createJointMessageQueue.clear();
	}
	m_messageQueueSemaphore.release();

	m_drawDebugInfoSemaphore.acquire();
	m_timestepAccumulator += a_deltaSec;
	if (m_timestepAccumulator > a_deltaSec)
	{
		m_timestepAccumulator -= a_deltaSec;
		m_physicsWorld->Step(TIMESTEP, VELOCITY_ITERATIONS, POSITION_ITERATIONS);
	}
	m_drawDebugInfoSemaphore.release();
}

void PhysicsSystem::initializeDebugDraw()
{
	m_debugDraw = new Box2DDebugDraw();
	m_debugDraw->initialize();
	m_debugDraw->SetFlags(b2Draw::e_shapeBit | b2Draw::e_jointBit);
	m_physicsWorld->SetDebugDraw(m_debugDraw);
}

void PhysicsSystem::drawDebugInfo(PerspectiveCamera& a_camera)
{
	m_drawDebugInfoSemaphore.acquire();
	m_physicsWorld->DrawDebugData();
	m_debugDraw->render(a_camera);
	m_drawDebugInfoSemaphore.release();
}