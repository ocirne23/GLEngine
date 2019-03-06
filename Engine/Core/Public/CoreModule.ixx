#include <stdio.h>
#include <iostream>
#include "CoreAPI.h"

export module CoreModule;

export void CORE_API bar()
{
	std::cout << "bar" << std::endl;
}

export void CORE_API foo();

export class CORE_API Thing;