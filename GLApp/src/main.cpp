#include "GLEngine.h"

#include "Utils/DeltaTimeMeasurer.h"

#if 1

#include "NetworkTest.h"

int main()
{
	GLEngine::initialize("GLApp", 192, 108, EWindowMode::WINDOWED);
	
	GLEngine::createThread("NetworkTestThread", [&]()
	{
		NetworkTest networkTest;
		DeltaTimeMeasurer deltaTimeMeasurer;
		while (!GLEngine::isShutdown())
		{
			GLEngine::processInput();
			networkTest.update(deltaTimeMeasurer.calcDeltaSec(GLEngine::getTimeMs()));
		}
	});

	while (!GLEngine::isShutdown())
		GLEngine::doMainThreadTick();

	GLEngine::finish();

	print("bai\n");
	GLEngine::sleep(1000);

	return 0;
}

#else

#include "TestScreen.h"

int main()
{
	GLEngine::initialize("GLApp", 1280, 720, EWindowMode::WINDOWED);
	
	GLEngine::createThread("RenderThread", [&]()
	{
		GLEngine::createGLContext();
		{
			TestScreen testScreen;
			DeltaTimeMeasurer deltaTimeMeasurer;
			while (!GLEngine::isShutdown())
			{
				GLEngine::processInput();
				testScreen.render(deltaTimeMeasurer.calcDeltaSec(GLEngine::getTimeMs()));
			}
		}
		GLEngine::destroyGLContext();
	});

	while (!GLEngine::isShutdown())
		GLEngine::doMainThreadTick();
	
	GLEngine::finish();

	return 0;
}
#endif
