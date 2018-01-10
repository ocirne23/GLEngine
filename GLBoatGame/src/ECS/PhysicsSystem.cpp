#include "ECS/PhysicsSystem.h"

#include "Physics/ContactListener.h"
#include "Graphics/Utils/Box2DDebugDraw.h"
#include "ECS/LogicSystem.h"

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
	m_components.resize(Entity::MAX_NUM_ENTITIES);
	print("size: %i\n", m_components.size());
	m_physicsWorld = new b2World(b2Vec2(0.0f, 0.0f));
	m_physicsWorld->SetContactListener(m_contactListener);
	/*
	CreateBodyMessage message;
	message.entity.index = 2;
	std::shared_ptr<b2Shape> circle = std::make_unique<b2CircleShape>();
	circle->m_radius = 3.0f;
	b2FixtureDef fixtureDef;
	message.addFixture(fixtureDef, circle);
	addCreateBodyMessage(message);*/
}

PhysicsSystem::~PhysicsSystem()
{
	SAFE_DELETE(m_contactListener);
	SAFE_DELETE(m_debugDraw);
	SAFE_DELETE(m_physicsWorld);
}

void PhysicsSystem::setupContactListener(LogicSystem& a_logicSystem)
{
	assert(m_contactListener == NULL);
	m_contactListener = new ContactListener(*this, a_logicSystem);
}

void PhysicsSystem::update(float a_deltaSec)
{
	m_messageQueueMutex.lock();
	{
		for (const CreateJointMessage& message : m_createJointMessageQueue)
		{
			assert(m_components[message.entity.index].body != NULL);
			b2Joint* joint = m_physicsWorld->CreateJoint(message.jointDef.get());
		}
		m_createJointMessageQueue.clear();

		for (const ApplyForceMessage& message : m_applyForceMessageQueue)
		{
			b2Body* body = m_components[message.entity.index].body;
			if (body)
			{
				body->ApplyForce(message.force, message.location, true);
			}
		}
		m_applyForceMessageQueue.clear();

		for (const DestroyBodyMessage& message : m_destroyBodyMessageQueue)
		{
			b2Body*& body = m_components[message.entity.index].body;
			if (body)
			{
				m_physicsWorld->DestroyBody(body);
				body = NULL;
			}
		}
		m_destroyBodyMessageQueue.clear();

		for (CreateBodyMessage& message : m_createBodyMessageQueue)
		{
			assert(m_components.at(message.entity.index).body == NULL);
			b2Body* body = m_physicsWorld->CreateBody(&message.bodyDef);
			int entityID = message.entity.getID();
			body->SetUserData(rcast<void*>(size_t(entityID)));
			for (uint i = 0; i < message.fixtureDefs.size(); ++i)
			{
				auto* fixture = &message.fixtureDefs[i].fixture;
				fixture->userData = rcast<void*>(size_t(i));
				body->CreateFixture(fixture);
			}
			m_components[message.entity.index].body = body;
		}
		m_createBodyMessageQueue.clear();
	}
	m_messageQueueMutex.unlock();

	m_timestepAccumulator += a_deltaSec;
	if (m_timestepAccumulator > a_deltaSec)
	{
		m_timestepAccumulator -= a_deltaSec;
		m_drawDebugInfoMutex.lock();
		m_physicsWorld->Step(TIMESTEP, VELOCITY_ITERATIONS, POSITION_ITERATIONS);
		m_drawDebugInfoMutex.unlock();
	}
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
	m_drawDebugInfoMutex.lock();
	m_physicsWorld->DrawDebugData();
	m_debugDraw->render(a_camera);
	m_drawDebugInfoMutex.unlock();
}

void PhysicsSystem::addCreateBodyMessage(const CreateBodyMessage& a_message)
{
	m_messageQueueMutex.lock();
	m_createBodyMessageQueue.push_back(a_message);
	m_messageQueueMutex.unlock();
}

void PhysicsSystem::addDestroyBodyMessage(const DestroyBodyMessage& a_message)
{
	m_messageQueueMutex.lock();
	m_destroyBodyMessageQueue.push_back(a_message);
	m_messageQueueMutex.unlock();
}

void PhysicsSystem::addApplyForceMessage(const ApplyForceMessage& a_message)
{
	m_messageQueueMutex.lock();
	m_applyForceMessageQueue.push_back(a_message);
	m_messageQueueMutex.unlock();
}

void CreateBodyMessage::addFixture(b2FixtureDef fixture, std::shared_ptr<b2Shape>& shape)
{
	fixture.shape = shape.get();
	FixtureDef def;
	def.fixture = fixture;
	def.shape = shape;
	fixtureDefs.push_back(def);
}
