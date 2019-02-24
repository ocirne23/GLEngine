#pragma once

#include "Core.h"
#include "GraphicsAPI.h"
#include "Core/Pimpl.h"

class IContext;
enum class EContextType;

enum class EWindowMode
{
	WINDOWED,
	BORDERLESS,
	FULLSCREEN,
	NONE
};

class Window;

class GRAPHICS_API IWindow
{
private:

	friend class IGraphics;
	IWindow(const char* name, uint width, uint height, uint xPos, uint yPos, const EWindowMode& mode);

public:

	explicit operator Window&() { return *m_impl; }

	//IContext createContext(const EContextType& type);
	void swapBuffer();

private:

	Pimpl<Window> m_impl;
};