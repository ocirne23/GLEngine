#pragma once

#include "Core.h"
#include "GraphicsAPI.h"

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
public:

	IWindow();
	IWindow(const IWindow&) = delete;
	IWindow(IWindow&& move);
	~IWindow();

	explicit operator Window&() { return *m_impl; }

	IContext createContext(const EContextType& type);
	void swapBuffer();

private:

	owner<Window*> m_impl = nullptr;
};