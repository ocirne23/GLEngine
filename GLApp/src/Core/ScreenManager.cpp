#include "ScreenManager.h"

#include "Screens/GameScreen.h"

#include "GLEngine.h"
#include "Graphics/Graphics.h"
#include "Graphics/WindowEventListener.h"

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

void ScreenManager::render(float a_deltaSec)
{
	if (m_currentScreen)
		m_currentScreen->render(a_deltaSec);
}

void ScreenManager::setScreen(EScreenType a_screenType)
{
	if (!m_screens[a_screenType])
	{
		switch (a_screenType)
		{
		case EScreenType_GAMESCREEN:
			m_screens[a_screenType] = new GameScreen(this);
			break;
		default:
			assert(0);
			break;
		}
	}

	swapScreen(m_screens[a_screenType]);
}

void ScreenManager::swapScreen(IScreen* a_screen)
{
	if (m_currentScreen)
		m_currentScreen->hide();
	a_screen->show(m_width, m_height);
	m_currentScreen = a_screen;
}

void ScreenManager::resize(uint a_width, uint a_height)
{
	m_width = a_width;
	m_height = a_height;
	if (m_currentScreen)
		m_currentScreen->resize(a_width, a_height);
}

void ScreenManager::quit()
{
	hasQuit = true;
}