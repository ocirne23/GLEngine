#include "GLEngine.h"

#include "Screens/TestScreen.h"
#include "AppUtils/DeltaTimeMeasurer.h"
#include "AppUtils/FPSMeasurer.h"

int main()
{
	GLEngine::initialize();

	GLEngine::initializeRenderThread([]()
	{
		FPSMeasurer fpsMeasurer(5.0f, [](const FPSMeasurer& measurer)
		{
			print("FPS: %i \t MS: %f\n", 
				  (uint) (measurer.getNumFramesPassed() / measurer.getTimeInterval()), 
				  measurer.getTimePassed() / measurer.getNumFramesPassed());
		});

		DeltaTimeMeasurer deltaTimeMeasurer;
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