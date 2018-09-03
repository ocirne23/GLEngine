#pragma once

#include "Core.h"
#include "Input/InputAPI.h"

class IInput;

class INPUT_API InputModule
{
public:

	static owner<IInput*> createInput();
	static void destroyInput(owner<IInput*> threading);

private:

	InputModule() {}
	~InputModule() {}
};