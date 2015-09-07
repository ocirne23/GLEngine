#include "GLEngine.h"

#include "Screens/TestScreen.h"

#include "AppUtils/DeltaTimeMeasurer.h"
#include "AppUtils/FPSMeasurer.h"

int main()
{
	GLEngine::initialize();

	const float fpsLogInterval = 5.0f;
	FPSMeasurer fpsMeasurer(fpsLogInterval, [](const FPSMeasurer& measurer)
	{
		print("FPS: %i \t MS: %f\n", measurer.getAvgFps(), measurer.getAvgMsPerFrame());
	});

	GLEngine::createThread("RenderThread", [&]()
	{
		GLEngine::createGLContext();
		DeltaTimeMeasurer deltaTimeMeasurer;
		TestScreen testScreen;
		while (!GLEngine::isShutdown())
		{
			GLEngine::doEngineTick();
			float deltaSec = deltaTimeMeasurer.calcDeltaSec(GLEngine::getTimeMs());
			testScreen.render(deltaSec);
			fpsMeasurer.tickFrame(deltaSec);
		}
		GLEngine::destroyGLContext();
	});

	while (!GLEngine::isShutdown())
		GLEngine::doMainThreadTick();

	GLEngine::finish();
	return 0;
}