#include "Input.h"

owner<IInput*> IInput::create()
{
	return new Input();
}