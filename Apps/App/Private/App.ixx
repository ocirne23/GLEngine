#include <stdio.h>
#include <iostream>

export module M1;

export class App
{
public:
	App()
	{
		printf("App");
		getchar();
	}
};