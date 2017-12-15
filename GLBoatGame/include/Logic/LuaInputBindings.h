#pragma once

#include <sol/sol.hpp>
#include "Input/EKey.h"

class LogicSystem;

class LuaInputBindings
{
public:

	LuaInputBindings(sol::state& a_lua, LogicSystem& a_logicSystem) 
		: m_logicSystem(a_logicSystem)
		, m_lua(a_lua) 
	{
	}

	void initialize();

	void update(float a_deltaSec);

	void keyDown(EKey key);
	void keyUp(EKey key);

private:

	LogicSystem& m_logicSystem;
	sol::state& m_lua;
	sol::function m_keyDown;
	sol::function m_keyUp;
	sol::function m_update;
};