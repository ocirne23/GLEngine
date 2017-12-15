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
	m.bodyDef.userData = table["bodyDef"]["userData"];

	

	return m;
}
