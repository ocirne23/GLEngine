#pragma once

#include <sol/sol.hpp>
#include "ECS/PhysicsSystem.h"

class LogicSystem;

class LuaPhysicsBindings
{
public:

	LuaPhysicsBindings(sol::state& a_lua, PhysicsSystem& a_physicsSystem, LogicSystem& a_logicSystem)
		: m_logicSystem(a_logicSystem)
		, m_lua(a_lua)
		, m_physicsSystem(a_physicsSystem) 
	{}
	void initialize();

	CreateBodyMessage createBodyMessage(sol::table table);
	DestroyBodyMessage destroyBodyMessage(sol::table table);
	ApplyForceMessage createApplyForceMessage(sol::table table);

private:

	sol::state& m_lua;
	PhysicsSystem& m_physicsSystem;
	LogicSystem& m_logicSystem;
};