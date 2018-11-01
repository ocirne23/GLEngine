#pragma once

#include "GraphicsAPI.h"
#include "Core/PimplPtr.h"

class GLFramebuffer;

class GRAPHICS_API IFramebuffer
{
public:

	IFramebuffer();
	explicit operator GLFramebuffer&() { return *m_impl; }

private:

	PimplPtr<GLFramebuffer> m_impl;
};