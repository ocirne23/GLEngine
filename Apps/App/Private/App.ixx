#include <stdio.h>
#include <iostream>

export module App2;

export void blah()
{
	printf("blah\n");
}

export class App
{
public:
	App()
	{
		printf("App\n");
		getchar();
	}
};