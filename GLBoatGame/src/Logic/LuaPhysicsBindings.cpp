#include "Logic/LuaPhysicsBindings.h"

#include "ECS/LogicSystem.h"

void LuaPhysicsBindings::initialize()
{
	sol::constructors<b2Vec2(), void(float, float)> ctor;
	sol::usertype<b2Vec2> utype(ctor, "x", &b2Vec2::x, "y", &b2Vec2::y);
	m_lua.set_usertype<b2Vec2>(utype);

	m_logicSystem.loadLuaScript(m_lua, "assets/lua/physics/physics.lua");
	
	m_lua["Physics"] = m_lua.create_named_table("Physics");
	m_lua["Physics"]["createBody"] = [this](sol::table args)
	{
		m_physicsSystem.addCreateBodyMessage(createBodyMessage(args));
		args["bodyDef"] = nullptr;
		args["fixtureDefs"] = nullptr;
	};
	m_lua["Physics"]["destroyBody"] = [this](sol::table args)
	{
		m_physicsSystem.addDestroyBodyMessage(destroyBodyMessage(args));
	};

	m_lua["Physics"]["applyForce"] = [this](Entity entity, b2Vec2 force, b2Vec2 location)
	{
		ApplyForceMessage m;
		m.entity = entity;
		m.force = force;
		m.location = location;
		m_physicsSystem.addApplyForceMessage(m);
	};

}

CreateBodyMessage LuaPhysicsBindings::createBodyMessage(sol::table a_table)
{
	CreateBodyMessage m;
	sol::optional<Entity> e = a_table["entity"];
	assert(e);
	m.entity = e.value();

	sol::optional<sol::table> bodyDefOpt = a_table["bodyDef"];
	assert(bodyDefOpt); 
	sol::table bodyDef = bodyDefOpt.value();

	m.bodyDef.active = bodyDef["active"];
	m.bodyDef.allowSleep = bodyDef["allowSleep"];
	m.bodyDef.angle = bodyDef["angle"];
	m.bodyDef.angularDamping = bodyDef["angularDamping"];
	m.bodyDef.angularVelocity = bodyDef["angularVelocity"];
	m.bodyDef.awake = bodyDef["awake"];
	m.bodyDef.bullet = bodyDef["bullet"];
	m.bodyDef.fixedRotation = bodyDef["fixedRotation"];
	m.bodyDef.gravityScale = bodyDef["gravityScale"];
	m.bodyDef.linearDamping = bodyDef["linearDamping"];
	m.bodyDef.linearVelocity = bodyDef["linearVelocity"];
	m.bodyDef.position = bodyDef["position"];
	m.bodyDef.type = bodyDef["bodyType"];
	m.bodyDef.userData;// = bodyDef["bodyDef"]["userData"];

	sol::table fixtureDefs = a_table["fixtureDefs"];
	for (auto fixtureDef : fixtureDefs)
	{
		CreateBodyMessage::FixtureDef fd;
		sol::table entry = fixtureDef.second;
		sol::table entryFilter = entry["filter"];
		sol::table entryShape = entry["shape"];
		b2Shape::Type shapeType = entryShape["type"];
		
		switch (shapeType)
		{
		case b2Shape::e_circle:
		{
			auto circle = std::make_shared<b2CircleShape>();
			circle->m_p = entryShape["position"];
			fd.shape = circle;
		}
			break;
		case b2Shape::e_polygon:
			assert(false);
			break;
		case b2Shape::e_edge:
			assert(false);
			break;
		case b2Shape::e_chain:
			assert(false);
			break;
		default:
			assert(false);
			break;
		}
		fd.shape->m_radius = entryShape["radius"];
		fd.fixture.density = entry["density"];
		fd.fixture.filter.categoryBits = entryFilter["categoryBits"];
		fd.fixture.filter.maskBits = entryFilter["maskBits"];
		fd.fixture.filter.groupIndex = entryFilter["groupIndex"];
		fd.fixture.friction = entry["friction"];
		fd.fixture.isSensor = entry["isSensor"];
		fd.fixture.restitution = entry["restitution"];
		fd.fixture.shape = fd.shape.get();
		fd.fixture.userData;// = entry["userData"];
		m.fixtureDefs.push_back(fd);
	}

	return m;
}

DestroyBodyMessage LuaPhysicsBindings::destroyBodyMessage(sol::table table)
{
	DestroyBodyMessage m;
	m.entity = table["entity"];
	return m;
}

ApplyForceMessage LuaPhysicsBindings::createApplyForceMessage(sol::table table)
{
	ApplyForceMessage m;
	m.entity = table["entity"];
	m.force = table["force"];
	m.location = table["location"];
	return m;
}
