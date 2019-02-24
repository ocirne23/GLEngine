#include "Graphics/IWindow.h"

#include "Window.h"

IWindow::IWindow(const char* name, uint width, uint height, uint xPos, uint yPos, const EWindowMode& mode)
	: m_impl(make_pimpl<Window>(name, width, height, xPos, yPos, mode))
{
}

void IWindow::swapBuffer()
{
	m_impl->swapBuffer();
}