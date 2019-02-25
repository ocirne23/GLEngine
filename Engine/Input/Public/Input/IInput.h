#pragma once

#include "Core.h"
#include "InputAPI.h"

class INPUT_API IInput
{
public:
	
	static owner<IInput*> create();
};