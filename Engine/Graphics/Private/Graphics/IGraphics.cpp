#include "Graphics/IGraphics.h"

#include "Graphics.h"
#include "Graphics/IWindow.h"

IGraphics::IGraphics()
	: m_impl(make_pimpl<Graphics>())
{
}

IWindow IGraphics::createWindow(const char* name, uint width, uint height, uint xPos, uint yPos, const EWindowMode& mode)
{
	return IWindow(name, width, height, xPos, yPos, mode);
}
