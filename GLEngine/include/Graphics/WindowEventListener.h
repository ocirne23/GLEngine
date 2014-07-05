#pragma once

#include "Core.h"

class WindowEventListener
{
public:
	virtual ~WindowEventListener() {}

	virtual void resize(uint width, uint height) = 0;
	virtual void quit() = 0;
};