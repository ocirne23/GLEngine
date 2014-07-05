#include "ScreenManager.h"

#include "..\Screens\GameScreen.h"

#include "GLEngine.h"
#include "Graphics\Graphics.h"
#include "Graphics\WindowEventListener.h"

ScreenManager::ScreenManager() : m_currentScreen(0), m_width(0), m_height(0), hasQuit(false)
{
	memset(m_screens, 0, sizeof(m_screens));
	m_currentScreen = NULL;
	m_width = GLEngine::graphics->getScreenWidth();
	m_height = GLEngine::graphics->getScreenHeight();

	GLEngine::graphics->registerWindowEventListener(this);
}

ScreenManager::~ScreenManager()
{
	GLEngine::graphics->unregisterWindowEventListener(this);

	if (m_currentScreen)
		m_currentScreen->hide();
}

void ScreenManager::render(float deltaSec)
{
	if (m_currentScreen)
		m_currentScreen->render(deltaSec);
}

void ScreenManager::setScreen(ScreenType screenType)
{
	if (!m_screens[screenType])
	{
		switch (screenType)
		{
		case ScreenType_GAMESCREEN:
			m_screens[screenType] = new GameScreen(this);
			break;
		default:
			assert(0);
			break;
		}
	}

	swapScreen(m_screens[screenType]);
}

void ScreenManager::swapScreen(IScreen* screen)
{
	if (m_currentScreen)
		m_currentScreen->hide();
	screen->show(m_width, m_height);
	m_currentScreen = screen;
}

void ScreenManager::resize(uint width, uint height)
{
	m_width = width;
	m_height = height;
	if (m_currentScreen)
		m_currentScreen->resize(width, height);
}

void ScreenManager::quit()
{
	hasQuit = true;
}