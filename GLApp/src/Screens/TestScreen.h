#pragma once

#include "3rdparty/entityx/entityx.h"
#include "3rdparty/rde/vector.h"
#include "Core.h"
#include "Input/Input.h"

class TestScreen
{
public:
	TestScreen();
	~TestScreen();

	void render(float deltaSec);

private:

	void keyDown(EKey key);

private:

	Input::KeyDownListener m_keyDownListener;
	Input::WindowQuitListener m_windowQuitListener;

	rde::vector<entityx::Entity> m_lightEntities;
	entityx::EntityX m_entityx;
};