#pragma once

#include "Core.h"
#include "../Core/IScreen.h"
#include "Input\KeyListener.h"
#include "Graphics\GL\Core\GLTexture.h"
#include "Graphics\GL\GLMesh.h"
#include "Graphics\GL\GLLightManager.h"
#include "Graphics\GL\Tech\ClusteredShading.h"
#include "Graphics\GL\Core\GLShader.h"

class GameScreen : public IScreen, public KeyListener
{
public:
	GameScreen(ScreenManager* screenManager);
	virtual ~GameScreen() OVERRIDE;

	virtual void render(float deltaSec) OVERRIDE;
	virtual void show(uint width, uint height) OVERRIDE;
	virtual void resize(uint width, uint height) OVERRIDE;
	virtual void hide() OVERRIDE;
	virtual bool keyDown(Key key) OVERRIDE;

private:

	GLTexture m_dfvTexture;
	GLMesh m_mesh;
	GLShader m_modelShader;
	GLLightManager m_lightManager;
	ClusteredShading m_clusteredShading;
};