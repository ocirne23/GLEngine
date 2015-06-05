#include "GLEngine.h"

#define DA
#ifdef DA
#include "Screens/TestScreen.h"
#else
#include "Screens/GLTestScreen.h"
#endif

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
#ifdef DA
		TestScreen testScreen;
#else
		GLTestScreen glTestScreen;
#endif
		while (!GLEngine::isShutdown())
		{
			GLEngine::doRenderThreadTick();
			float deltaSec = deltaTimeMeasurer.calcDeltaSec(GLEngine::getTimeMs());
#ifdef DA
			testScreen.render(deltaSec);
#else
			glTestScreen.render(deltaSec);
#endif
			fpsMeasurer.tickFrame(deltaSec);
		}
	});

	while (!GLEngine::isShutdown())
	{
		GLEngine::doMainThreadTick();
	}
	return 0;
}