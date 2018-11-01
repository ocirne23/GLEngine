#include "Graphics/IWindow.h"

#include "Window.h"
#include "Graphics/IContext.h"

IWindow::IWindow(const char* name, uint width, uint height, uint xPos, uint yPos, const EWindowMode& mode)
	: m_impl(make_pimpl<Window>(name, width, height, xPos, yPos, mode))
{
}
/*
IContext IWindow::createContext(const EContextType& type)
{
	;
}*/

void IWindow::swapBuffer()
{
	m_impl->swapBuffer();
}