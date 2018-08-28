#pragma once

#include "Core.h"
#include "GraphicsAPI.h"

class IWindow;
enum class EWindowMode;

class GRAPHICS_API Graphics final
{
public:

	Graphics() {}
	Graphics(const Graphics&) = delete;
	~Graphics() {}

	owner<IWindow*> createWindow(const char* name, uint width, uint height, uint xPos, uint yPos, const EWindowMode& mode);
	void destroyWindow(owner<IWindow*> window);
};