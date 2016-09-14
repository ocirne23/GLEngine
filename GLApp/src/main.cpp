#include "GLEngine.h"

#include "TestScreen.h"
#include "Utils/DeltaTimeMeasurer.h"

int main()
{
	GLEngine::initialize("GLApp", 1280, 720, EWindowMode::WINDOWED);
	
	GLEngine::createThread("RenderThread", [&]()
	{
#if 0
		GLEngine::test();
#else
		GLEngine::createGLContext();
		{
			TestScreen testScreen;
			DeltaTimeMeasurer deltaTimeMeasurer;
			while (!GLEngine::isShutdown())
			{
				GLEngine::doEngineTick();
				testScreen.render(deltaTimeMeasurer.calcDeltaSec(GLEngine::getTimeMs()));
			}
		}
		GLEngine::destroyGLContext();
#endif
	});

	while (!GLEngine::isShutdown())
		GLEngine::doMainThreadTick();
	
	GLEngine::finish();

	return 0;
}