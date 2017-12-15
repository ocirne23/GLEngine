#pragma once

#include "Input/Input.h"
#include "Input/InputListener.h"
#include "Logic/LuaPhysicsBindings.h"
#include "Logic/LuaInputBindings.h"
#include "Logic/LuaEntityBindings.h"

#include <sol/sol.hpp>

class BoatGame;

class LogicSystem
{
public:

	LogicSystem(BoatGame& boatGame);


	void update(float deltaSec);

	static sol::protected_function_result loadLuaScript(sol::state& a_lua, const char* a_filePath);

private:

	void initializeLuaState();

private:

	BoatGame& m_boatGame;

	sol::state m_lua;

	Input::KeyDownListener m_keyDownListener;
	Input::KeyUpListener m_keyUpListener;
	Input::MouseDownListener m_mouseDownListener;
	Input::MouseUpListener m_mouseUpListener;
	Input::MouseMovedListener m_mouseMovedListener;
	Input::MouseScrolledListener m_mouseScrolledListener;

	LuaPhysicsBindings m_physicsBindings;
	LuaInputBindings m_inputBindings;
	LuaEntityBindings m_entityBindings;
};