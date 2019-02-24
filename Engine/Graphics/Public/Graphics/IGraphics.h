#pragma once

#include "Core.h"
#include "GraphicsAPI.h"
#include "Core/Pimpl.h"

class IWindow;
enum class EWindowMode;
class Graphics;

class GRAPHICS_API IGraphics
{
public:

	IGraphics();
	explicit operator Graphics&() { return *m_impl; }

	IWindow createWindow(const char* name, uint width, uint height, uint xPos, uint yPos, const EWindowMode& mode);

private:

	Pimpl<Graphics> m_impl;
};