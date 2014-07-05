#pragma once

class ScreenManager;

class IScreen
{
public:
	IScreen(ScreenManager* screenManager) : m_screenManager(screenManager) {}
	virtual ~IScreen() {}

	virtual void render(float deltaSec) = 0;
	virtual void show(uint width, uint height) = 0;
	virtual void resize(uint width, uint height) = 0;
	virtual void hide() = 0;

protected:
	ScreenManager* m_screenManager;
private:
};