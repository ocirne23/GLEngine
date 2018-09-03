#pragma once

#include "Graphics/IFramebuffer.h"

class GLFramebuffer : public IFramebuffer
{
public:


private:

	friend class GLContext;
	GLFramebuffer() {}
	~GLFramebuffer() override {}
	GLFramebuffer(const GLFramebuffer&) = delete;
};