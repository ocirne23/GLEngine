#include "GLEngine.h"
#include "Utils/DeltaTimeMeasurer.h"

#include "Input/Input.h"

#include "BoatGame.h"

int main()
{
	GLEngine::initialize("GLApp", 1280, 720, EWindowMode::WINDOWED);
	{
		BoatGame boatGame;
	}
	GLEngine::finish();

	return 0;
}