#include "Logic/LuaPhysicsBindings.h"

void LuaPhysicsBindings::initialize()
{
	sol::constructors<b2Vec2(), void(float, float)> ctor;
	sol::usertype<b2Vec2> utype(ctor, "x", &b2Vec2::x, "y", &b2Vec2::y);
	m_lua.set_usertype<b2Vec2>(utype);

	m_lua.set_function("createBody", [this](sol::table args)
	{
		m_physicsSystem.addCreateBodyMessage(createBodyMessage(args));
		args["bodyDef"] = nullptr;
	});

	m_lua.set_function("destroyBody", [this](sol::table args)
	{
		m_physicsSystem.addDestroyBodyMessage(destroyBodyMessage(args));
	});
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
	m.bodyDef.angularDamping = bodyDef["angularDampening"];
	m.bodyDef.angularVelocity = bodyDef["angularVelocity"];
	m.bodyDef.awake = bodyDef["awake"];
	m.bodyDef.bullet = bodyDef["bullet"];
	m.bodyDef.fixedRotation = bodyDef["fixedRotation"];
	m.bodyDef.gravityScale = bodyDef["gravityScale"];
	m.bodyDef.linearDamping = bodyDef["linearDamping"];
	m.bodyDef.linearVelocity = bodyDef["linearVelocity"];
	m.bodyDef.position = bodyDef["position"];
	m.bodyDef.type = a_table["bodyDef"]["type"];
	m.bodyDef.userData;// = table["bodyDef"]["userData"];

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
	Entity entity = table["entity"];
	return DestroyBodyMessage({ entity });
}
