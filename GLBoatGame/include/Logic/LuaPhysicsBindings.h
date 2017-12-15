#pragma once

#include <sol/sol.hpp>
#include "ECS/PhysicsSystem.h"

class LuaPhysicsBindings
{
public:

	LuaPhysicsBindings(sol::state& a_lua, PhysicsSystem& a_physicsSystem) 
		: m_lua(a_lua)
		, m_physicsSystem(a_physicsSystem) 
	{}
	void initialize();

private:

	CreateBodyMessage createBodyMessage(sol::table table);

private:

	sol::state& m_lua;
	PhysicsSystem& m_physicsSystem;
};