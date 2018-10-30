#include "Graphics/IWindow.h"

#include "Window.h"

IWindow::IWindow()
	: m_impl(new Window("", 512, 512, 0, 0, EWindowMode::WINDOWED))
{
}

IWindow::~IWindow()
{
	SAFE_DELETE(m_impl);
}

IWindow::IWindow(IWindow&& move)
{
	m_impl = move.m_impl;
	move.m_impl = nullptr;
}