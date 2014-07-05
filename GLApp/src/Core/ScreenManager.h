#pragma once

#include "Graphics\WindowEventListener.h"

class IScreen;

enum ScreenType
{
	ScreenType_GAMESCREEN,
	ScreenType_NUM_SCREENTYPES
};

class ScreenManager : public WindowEventListener
{
public:
	ScreenManager();
	virtual ~ScreenManager() OVERRIDE;

	bool hasQuitWindow() const { return hasQuit; }

	void setScreen(ScreenType screenType);
	void render(float deltaSec);

	virtual void resize(uint width, uint height) OVERRIDE;
	virtual void quit() OVERRIDE;

private:

	void swapScreen(IScreen* screen);

private:
	IScreen* m_currentScreen;
	IScreen* m_screens[ScreenType_NUM_SCREENTYPES];

	uint m_width;
	uint m_height;
	bool hasQuit;
};