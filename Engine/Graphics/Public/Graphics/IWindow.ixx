#include "Core.h"
#include "GraphicsAPI.h"

export module Graphics.IWindow;

import Graphics.IContext;

export enum class EWindowMode
{
	WINDOWED,
	BORDERLESS,
	FULLSCREEN,
	NONE
};

export class GRAPHICS_API IWindow
{
public:

	virtual owner<IContext*> createContext(const EContextType& contextType) = 0;
	virtual void             destroyContext(owner<IContext*>& context) = 0;
	virtual void             swapBuffers() = 0;
	virtual void*            getSDLWindow() = 0;
	virtual void*            getHDC() = 0;
	virtual void*            getHWND() = 0;
	virtual void*            getHINSTANCE() = 0;
	virtual void*            getWNDPROC() = 0;
	virtual uint             getWidth() const = 0;
	virtual uint             getHeight() const = 0;

protected:

	virtual ~IWindow() {}
};