#pragma once

#include "Core.h"
#include "Utils/ConcurrentQueue.h"
#include "Utils/Mutex.h"
#include "ECS/Entity.h"

#include <Box2D/Box2D.h>
#include <EASTL/fixed_vector.h>

class BoatGame;
class PerspectiveCamera;
class Box2DDebugDraw;
class ContactListener;
class LogicSystem;

struct CreateBodyMessage
{
	CreateBodyMessage() {}
	Entity entity;
	b2BodyDef bodyDef;

	struct FixtureDef
	{
		b2FixtureDef fixture;
		std::shared_ptr<b2Shape> shape;
	};
	eastl::vector<FixtureDef> fixtureDefs;

	void addFixture(b2FixtureDef fixture, std::shared_ptr<b2Shape>& shape);
};

struct CreateJointMessage
{
	CreateJointMessage() {}
	Entity entity;
	std::unique_ptr<b2JointDef> jointDef;
};

struct PhysicsComponent
{
	b2Body* body = NULL;
};

struct DestroyBodyMessage
{
	Entity entity;
};

struct ApplyForceMessage
{
	Entity entity;
	b2Vec2 force;
	b2Vec2 location;
};

class PhysicsSystem
{
public:

	PhysicsSystem(BoatGame& boatGame);
	~PhysicsSystem();

	void setupContactListener(LogicSystem& logicSystem);
	void update(float a_deltaSec);

	void initializeDebugDraw();
	void drawDebugInfo(PerspectiveCamera& a_camera);
	void addCreateBodyMessage(const CreateBodyMessage& message);
	void addDestroyBodyMessage(const DestroyBodyMessage& message);
	void addApplyForceMessage(const ApplyForceMessage& message);

private:

	BoatGame& m_boatGame;

	owner<Box2DDebugDraw*> m_debugDraw        = NULL;
	owner<ContactListener*> m_contactListener = NULL;
	owner<b2World*> m_physicsWorld            = NULL;
	float m_timestepAccumulator               = 0.0f;

	Mutex m_drawDebugInfoMutex;
	Mutex m_messageQueueMutex;
	eastl::fixed_vector<PhysicsComponent, Entity::MAX_NUM_ENTITIES, false> m_components;
	eastl::vector<CreateBodyMessage> m_createBodyMessageQueue;
	eastl::vector<CreateJointMessage> m_createJointMessageQueue;
	eastl::vector<DestroyBodyMessage> m_destroyBodyMessageQueue;
	eastl::vector<ApplyForceMessage> m_applyForceMessageQueue;
};