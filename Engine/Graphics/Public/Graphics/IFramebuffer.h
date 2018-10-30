#pragma once

#include "GraphicsAPI.h"

class GLFramebuffer;

class GRAPHICS_API IFramebuffer
{
public:

	IFramebuffer();
	IFramebuffer(const IFramebuffer&) = delete;
	~IFramebuffer();
	explicit operator GLFramebuffer&() { return *m_impl; }

private:

	owner<GLFramebuffer*> m_impl = nullptr;
};