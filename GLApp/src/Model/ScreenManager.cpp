#include "ScreenManager.h"

#include "Screens/TestScreen.h"

#include "GLEngine.h"
#include "Graphics/Graphics.h"

ScreenManager::ScreenManager()
{
	memset(m_screens, 0, sizeof(m_screens));
	REGISTER_LISTENER(GLEngine::graphics, &Graphics::windowQuitListenerRegister, this, &ScreenManager::quit);
}

ScreenManager::~ScreenManager()
{
	UNREGISTER_LISTENER(GLEngine::graphics, &Graphics::windowQuitListenerUnregister, this);

	for (IScreen* screen : m_screens)
		SAFE_DELETE(screen);
}

void ScreenManager::quit()
{
	m_hasQuit = true;
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
		case EScreenType_TESTSCREEN:
			m_screens[a_screenType] = new TestScreen();
			m_screens[a_screenType]->m_screenManager = this;
			break;
		default:
			assert(false);
			break;
		}
	}

	swapScreen(m_screens[a_screenType]);
}

void ScreenManager::swapScreen(IScreen* a_screen)
{
	if (m_currentScreen)
		m_currentScreen->hide();
	a_screen->show();
	m_currentScreen = a_screen;
}