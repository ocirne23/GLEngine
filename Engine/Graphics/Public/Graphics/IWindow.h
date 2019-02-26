#pragma once

#include "Core.h"
#include "GraphicsAPI.h"

enum class EWindowMode
{
	WINDOWED,
	BORDERLESS,
	FULLSCREEN,
	NONE
};

struct SDL_Window;
class IContext;
enum class EContextType;

class GRAPHICS_API IWindow
{
public:

	virtual owner<IContext*> createContext(const EContextType& contextType) = 0;
	virtual void             destroyContext(owner<IContext*>& context) = 0;
	virtual void             swapBuffers() = 0;
	virtual SDL_Window*      getSDLWindow() = 0;
	virtual void*            getHDC() = 0;
	virtual void*            getHWND() = 0;
	virtual void*            getHINSTANCE() = 0;
	virtual void*            getWNDPROC() = 0;
	virtual uint             getWidth() const = 0;
	virtual uint             getHeight() const = 0;

protected:

	friend class Graphics;
	virtual ~IWindow() {}
};