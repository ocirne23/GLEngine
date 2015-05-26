#include "GLEngine.h"

#include "Screens/TestScreen.h"
#include "AppUtils/DeltaTimeMeasurer.h"
#include "AppUtils/FPSMeasurer.h"

int main()
{
	GLEngine::initialize();

	DeltaTimeMeasurer deltaTimeMeasurer;
	const float fpsLogInterval = 5.0f;
	FPSMeasurer fpsMeasurer(fpsLogInterval, [](const FPSMeasurer& measurer)
	{
		print("FPS: %i \t MS: %f\n", measurer.getAvgFps(), measurer.getAvgMsPerFrame());
	});

	GLEngine::createRenderThread([&]()
	{
		TestScreen testScreen;
		while (!GLEngine::isShutdown())
		{
			GLEngine::doRenderThreadTick();
			float deltaSec = deltaTimeMeasurer.calcDeltaSec(GLEngine::getTimeMs());
			testScreen.render(deltaSec);
			fpsMeasurer.tickFrame(deltaSec);
		}
	});

	while (!GLEngine::isShutdown())
	{
		GLEngine::doMainThreadTick();
	}
	return 0;
}