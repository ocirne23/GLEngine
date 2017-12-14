#pragma once

#include "Input/Input.h"
#include "Input/InputListener.h"

#include <sol/sol.hpp>

class BoatGame;

class LogicSystem
{
public:

	LogicSystem(BoatGame& boatGame);

	void update(float deltaSec);



private:

	BoatGame& m_boatGame;

	sol::state m_lua;

	Input::KeyDownListener m_keyDownListener;
	Input::KeyUpListener m_keyUpListener;
	Input::MouseDownListener m_mouseDownListener;
	Input::MouseUpListener m_mouseUpListener;
	Input::MouseMovedListener m_mouseMovedListener;
	Input::MouseScrolledListener m_mouseScrolledListener;
};