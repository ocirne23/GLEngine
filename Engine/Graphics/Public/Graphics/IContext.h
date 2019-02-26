#pragma once

#include "Core.h"
#include "GraphicsAPI.h"

enum class EContextType
{
	OPENGL
};

class GRAPHICS_API IContext
{
public:

	virtual EContextType getContextType() const = 0;

protected:

	friend class Window;
	virtual ~IContext() {}
};