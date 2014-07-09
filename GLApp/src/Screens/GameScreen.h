#pragma once

#include "Core.h"
#include "../Core/IScreen.h"
#include "Input\KeyListener.h"

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
};