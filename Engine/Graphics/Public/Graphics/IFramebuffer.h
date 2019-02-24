#pragma once

#include "GraphicsAPI.h"
#include "Core/Pimpl.h"

class GLFramebuffer;

class GRAPHICS_API IFramebuffer
{
public:

	IFramebuffer();
	explicit operator GLFramebuffer&() { return *m_impl; }

private:

	Pimpl<GLFramebuffer> m_impl;
};