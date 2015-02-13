#pragma once

#include "Core.h"
#include "Model/IScreen.h"
#include "Input/KeyListener.h"

#include "entityx/entityx.h"

class GLMesh;
class PerspectiveCamera;

class TestScreen : public IScreen, public KeyListener
{
public:
	TestScreen();
	~TestScreen();

	virtual void render(float deltaSec) OVERRIDE;
	virtual void show(uint width, uint height) OVERRIDE;
	virtual void resize(uint width, uint height) OVERRIDE;
	virtual void hide() OVERRIDE;

	virtual bool keyDown(Key key) OVERRIDE;

private:

	entityx::EntityX m_entityx;
	GLMesh* m_mesh;
	PerspectiveCamera* m_camera;
};