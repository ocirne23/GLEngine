#pragma once

#include <sol/sol.hpp>

class EntitySystem;
class LogicSystem;

class LuaEntityBindings
{
public:

	LuaEntityBindings(sol::state& a_lua, EntitySystem& a_entitySystem)
	: m_lua(a_lua)
	, m_entitySystem(a_entitySystem)
	{
	}

	void initialize();

private:

	sol::state& m_lua;
	EntitySystem& m_entitySystem;
};