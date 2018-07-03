#pragma once

#include "Graphics/IContext.h"

class GLContext : public IContext
{
public:

	GLContext();
	~GLContext();
	GLContext(const GLContext&) = delete;
};