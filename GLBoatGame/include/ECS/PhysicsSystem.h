#pragma once

#include "Core.h"
#include "Utils/ConcurrentQueue.h"
#include "Utils/Semaphore.h"
#include "ECS/Entity.h"

#include <Box2D/Box2D.h>
#include <EASTL/fixed_vector.h>

class BoatGame;
class PerspectiveCamera;
class Box2DDebugDraw;
class ContactListener;

struct CreateBodyMessage
{
	CreateBodyMessage() {}
	EntityID id;
	b2BodyDef bodyDef;
	eastl::vector<b2FixtureDef> fixtureDefs;
};

struct CreateJointMessage
{
	CreateJointMessage() {}
	EntityID id;
	std::shared_ptr<b2JointDef> jointDef;
};

struct PhysicsComponent
{
	b2Body* body = NULL;
};

class PhysicsSystem
{
public:

	PhysicsSystem(BoatGame& boatGame);
	~PhysicsSystem();

	void update(float a_deltaSec);

	void initializeDebugDraw();
	void drawDebugInfo(PerspectiveCamera& a_camera);

private:

	BoatGame& m_boatGame;

	owner<Box2DDebugDraw*> m_debugDraw        = NULL;
	owner<ContactListener*> m_contactListener = NULL;
	owner<b2World*> m_physicsWorld            = NULL;
	float m_timestepAccumulator               = 0.0f;

	Semaphore m_semaphore;
	eastl::fixed_vector<PhysicsComponent, Entity::MAX_NUM_ENTITIES, false> m_components;
	ConcurrentQueue<CreateBodyMessage> m_createBodyMessageQueue;
	ConcurrentQueue<CreateJointMessage> m_createJointMessageQueue;
};