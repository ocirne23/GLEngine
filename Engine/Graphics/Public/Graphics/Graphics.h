#pragma once

#include "Core.h"

class IWindow;
enum class EWindowMode;

class Graphics
{
public:

	virtual owner<IWindow*> createWindow(const char* name, uint width, uint height, uint xPos, uint yPos, const EWindowMode& mode) = 0;

};