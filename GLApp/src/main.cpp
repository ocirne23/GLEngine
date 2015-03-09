#include "GLEngine.h"

#include "Model/ScreenManager.h"
#include "Utils/JobScheduler.h"

int main()
{
	GLEngine::initialize();
	{
		ScreenManager screenManager;

		GLEngine::initializeRenderThread([&]()
		{
			JobScheduler jobScheduler;

			const float fpsLogDelay = 5.0f;
			uint startTime = GLEngine::getTimeMs();
			uint fpsCounter = 0;
			float fpsTimeAccumulator = 0.0f;

			jobScheduler.addJob(fpsLogDelay, [&](void* ptr)
			{
				print("FPS: %i \t MS: %f\n", (uint) (fpsCounter / fpsLogDelay), fpsTimeAccumulator / fpsCounter);
				fpsTimeAccumulator = 0.0f;
				fpsCounter = 0;
				return EJobRepeat_REPEAT;
			});

			screenManager.setScreen(EScreenType_TESTSCREEN);

			while (!screenManager.hasQuit())
			{
				uint currentTime = GLEngine::getTimeMs();
				float deltaSec = (currentTime - startTime) / 1000.0f;
				startTime = currentTime;

				fpsCounter++;
				fpsTimeAccumulator += deltaSec;

				screenManager.render(deltaSec);
				jobScheduler.update(deltaSec);
			}
		});

		while (!screenManager.hasQuit())
		{
			GLEngine::doMainThreadTick();
		}

	}
	GLEngine::shutdown();
	
	return 0;
}