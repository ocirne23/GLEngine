#pragma once

#include "Screens/EScreenType.h"

class IScreen;

class ScreenManager
{
public:
	ScreenManager();
	~ScreenManager();

	void setScreen(EScreenType screenType);
	void render(float deltaSec);
	void quit();
	bool hasQuit() { return m_hasQuit; }

private:

	void swapScreen(IScreen* screen);

private:

	IScreen* m_screens[EScreenType_NUM_SCREENTYPES];
	IScreen* m_currentScreen	= NULL;
	bool m_hasQuit				= false;
};