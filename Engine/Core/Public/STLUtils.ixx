#include <stdio.h>
#include <iostream>
#include "CoreAPI.h"

export void CORE_API foo()
{
	std::cout << "foo" << std::endl;
}

export class CORE_API Thing
{
public:
	void derp()
	{
		std::cout << "derp" << std::endl;
	}
private:
	int da;
};