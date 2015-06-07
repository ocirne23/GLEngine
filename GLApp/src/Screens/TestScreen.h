#pragma once

#include "Core.h"
#include "Input/Input.h"
#include "3rdparty/entityx/entityx.h"

class GLMesh;
class PerspectiveCamera;

class TestScreen
{
public:
	TestScreen();
	~TestScreen();

	void render(float deltaSec);

private:

	void keyDown(EKey key);

private:

	entityx::EntityX m_entityx;
	GLMesh* m_dragon			= NULL;
	GLMesh* m_building          = NULL;
	GLMesh* m_skybox            = NULL;
	PerspectiveCamera* m_camera = NULL;

	Input::KeyDownListener m_keyDownListener;
};