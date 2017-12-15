#include "PhysicsBindings.h"

CreateBodyMessage PhysicsBindings::createBodyMessage(sol::table table)
{
	CreateBodyMessage m;
	m.bodyDef.active = table["bodyDef"]["active"];
	m.bodyDef.allowSleep = table["bodyDef"]["allowSleep"];
	m.bodyDef.angle = table["bodyDef"]["angle"];
	m.bodyDef.angularDamping = table["bodyDef"]["angularDampening"];
	m.bodyDef.angularVelocity = table["bodyDef"]["angularVelocity"];
	m.bodyDef.awake = table["bodyDef"]["awake"];
	m.bodyDef.bullet = table["bodyDef"]["bullet"];
	m.bodyDef.fixedRotation = table["bodyDef"]["fixedRotation"];
	m.bodyDef.gravityScale = table["bodyDef"]["gravityScale"];
	m.bodyDef.linearDamping = table["bodyDef"]["linearDamping"];
	m.bodyDef.linearVelocity = table["bodyDef"]["linearVelocity"];
	m.bodyDef.position = table["bodyDef"]["position"];
	m.bodyDef.type = table["bodyDef"]["type"];
	m.bodyDef.userData;// = table["bodyDef"]["userData"];

	sol::table fixtureDefs = table["fixtureDefs"];
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
