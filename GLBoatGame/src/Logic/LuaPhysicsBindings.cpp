#include "Logic/LuaPhysicsBindings.h"

void LuaPhysicsBindings::initialize()
{
	sol::constructors<b2Vec2(), void(float, float)> ctor;
	sol::usertype<b2Vec2> utype(ctor, "x", &b2Vec2::x, "y", &b2Vec2::y);
	m_lua.set_usertype<b2Vec2>(utype);

	m_lua.set_function("spawnBody", [this](sol::table args)
	{
		CreateBodyMessage message = createBodyMessage(args);
		m_physicsSystem.addCreateBodyMessage(message);
	});
}

CreateBodyMessage LuaPhysicsBindings::createBodyMessage(sol::table a_table)
{
	CreateBodyMessage m;
	m.bodyDef.active = a_table["bodyDef"]["active"];
	m.bodyDef.allowSleep = a_table["bodyDef"]["allowSleep"];
	m.bodyDef.angle = a_table["bodyDef"]["angle"];
	m.bodyDef.angularDamping = a_table["bodyDef"]["angularDampening"];
	m.bodyDef.angularVelocity = a_table["bodyDef"]["angularVelocity"];
	m.bodyDef.awake = a_table["bodyDef"]["awake"];
	m.bodyDef.bullet = a_table["bodyDef"]["bullet"];
	m.bodyDef.fixedRotation = a_table["bodyDef"]["fixedRotation"];
	m.bodyDef.gravityScale = a_table["bodyDef"]["gravityScale"];
	m.bodyDef.linearDamping = a_table["bodyDef"]["linearDamping"];
	m.bodyDef.linearVelocity = a_table["bodyDef"]["linearVelocity"];
	m.bodyDef.position = a_table["bodyDef"]["position"];
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
