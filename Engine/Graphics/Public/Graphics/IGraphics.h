#pragma once

#include "Core.h"
#include "GraphicsAPI.h"

class IWindow;
enum class EWindowMode;
class ICamera;

class Graphics;

class GRAPHICS_API IGraphics
{
public:

	IGraphics();
	IGraphics(const IGraphics&) = delete;
	IGraphics(IGraphics&& move);
	~IGraphics();
	explicit operator Graphics&() { return *m_impl; }

	IWindow createWindow(const char* name, uint width, uint height, uint xPos, uint yPos, const EWindowMode& mode);

private:

	owner<Graphics*> m_impl = nullptr;
};