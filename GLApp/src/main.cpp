#include "GLEngine.h"

#include "AppUtils/DeltaTimeMeasurer.h"
#include "Screens/DBTestScreen.h"

int main()
{
	GLEngine::initialize();
	
	GLEngine::createThread("RenderThread", [&]()
	{
		GLEngine::createGLContext();
		DBTestScreen testScreen;
		DeltaTimeMeasurer deltaTimeMeasurer;
		while (!GLEngine::isShutdown())
		{
			GLEngine::doEngineTick();
			testScreen.render(deltaTimeMeasurer.calcDeltaSec(GLEngine::getTimeMs()));
		}
		GLEngine::destroyGLContext();
	});

	while (!GLEngine::isShutdown())
		GLEngine::doMainThreadTick();
	
	GLEngine::finish();
	return 0;
}