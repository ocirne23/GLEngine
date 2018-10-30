#include "Graphics/IGraphics.h"

#include "Graphics.h"
#include "Graphics/IWindow.h"

IGraphics::IGraphics()
	: m_impl(new Graphics())
{
}

IGraphics::IGraphics(IGraphics&& move)
{
	m_impl = move.m_impl;
	move.m_impl = nullptr;
}

IGraphics::~IGraphics()
{
	SAFE_DELETE(m_impl);
}

IWindow IGraphics::createWindow(const char* name, uint width, uint height, uint xPos, uint yPos, const EWindowMode& mode)
{
	return IWindow();
}
