#pragma once

#include "Core.h"
#include "Model/IScreen.h"
#include "Input/Key.h"
#include "entityx/entityx.h"

class GLMesh;
class PerspectiveCamera;

class TestScreen : public IScreen
{
public:
	TestScreen();
	~TestScreen();

	virtual void render(float deltaSec) OVERRIDE;
	virtual void show() OVERRIDE;
	virtual void hide() OVERRIDE;

private:

	bool keyDown(Key key);

private:

	entityx::EntityX m_entityx;
	GLMesh* m_building			= NULL;
	GLMesh* m_skybox			= NULL;
	PerspectiveCamera* m_camera	= NULL;
};