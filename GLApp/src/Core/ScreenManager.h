#pragma once

#include "Graphics/WindowEventListener.h"
#include "Screens/EScreenType.h"

class IScreen;

class ScreenManager : public WindowEventListener
{
public:
	ScreenManager();
	virtual ~ScreenManager() OVERRIDE;

	bool hasQuitWindow() const { return hasQuit; }

	void setScreen(EScreenType screenType);
	void render(float deltaSec);

	virtual void resize(uint width, uint height) OVERRIDE;
	virtual void quit() OVERRIDE;

private:

	void swapScreen(IScreen* screen);

private:

	IScreen* m_currentScreen;
	IScreen* m_screens[EScreenType_NUM_SCREENTYPES];

	uint m_width;
	uint m_height;
	bool hasQuit;
};