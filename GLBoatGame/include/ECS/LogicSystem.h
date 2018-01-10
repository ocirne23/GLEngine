#pragma once

#include "Input/Input.h"
#include "Input/InputListener.h"
#include "Logic/LuaPhysicsBindings.h"
#include "Logic/LuaInputBindings.h"
#include "Logic/LuaEntityBindings.h"
#include "Physics/ContactListener.h"
#include "Utils/Mutex.h"

#include <sol/sol.hpp>

class BoatGame;

class LogicSystem
{
public:

	LogicSystem(BoatGame& boatGame);


	void update(float deltaSec);

	sol::protected_function_result loadLuaScript(sol::state& a_lua, const char* a_filePath);

	void addBeginContactMessage(const ContactMessage& contactMessage);
	void addEndContactMessage(const ContactMessage& contactMessage);

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

	Mutex m_contactMutex;
	eastl::vector<ContactMessage> m_beginContactMessages;
	eastl::vector<ContactMessage> m_endContactMessages;
};