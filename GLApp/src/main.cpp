#include "GLEngine.h"

#include "Core/ScreenManager.h"
#include "Screens/GameScreen.h"

int MAIN(int a_argc, char* a_argv[])
{
	GLEngine::initialize();
	{
		ScreenManager screenManager;
		screenManager.setScreen(EScreenType_GAMESCREEN);

		uint startTime = GLEngine::getTimeMs();
		uint fpsCounter = 0;
		float fpsTimeAccumulator = 0.0f;
		const float fpsLogDelay = 5.0f;

		while (!screenManager.hasQuitWindow())
		{
			GLEngine::doEngineTick();

			uint currentTime = GLEngine::getTimeMs();
			float deltaSec = (currentTime - startTime) / 1000.0f;
			startTime = currentTime;

			fpsCounter++;
			fpsTimeAccumulator += deltaSec;
			if (fpsTimeAccumulator > fpsLogDelay)
			{
				print("FPS: %i \t MS: %f\n", (uint) (fpsCounter / fpsLogDelay), fpsTimeAccumulator / fpsCounter);
				fpsTimeAccumulator = 0.0f;
				fpsCounter = 0;
			}

			screenManager.render(deltaSec);
		}
	}
	GLEngine::shutdown();
	return 0;
}