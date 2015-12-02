#pragma once

#include "entityx/entityx.h"
#include "Core.h"
#include "Input/Input.h"
#include "EASTL/vector.h"

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

	eastl::vector<entityx::Entity> m_lightEntities;
	entityx::EntityX m_entityx;
};