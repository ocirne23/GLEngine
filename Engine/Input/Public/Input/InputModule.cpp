#include "Input/InputModule.h"

#include "Input.h"

owner<IInput*> InputModule::createInput()
{
	return owner<IInput*>(new Input());
}

void InputModule::destroyInput(owner<IInput*> input)
{
	assert(input);
	delete static_cast<Input*>(input);
}
