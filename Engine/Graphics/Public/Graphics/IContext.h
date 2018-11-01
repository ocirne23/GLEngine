#pragma once

#include "Core.h"
#include "GraphicsAPI.h"
#include "Core/PimplPtr.h"
#include <glm/fwd.hpp>

enum class EContextType
{
	OPENGL
};

class GLContext;

class GRAPHICS_API IContext
{
private:

	friend class IWindow;
	IContext(const EContextType& type);
	PimplPtr<GLContext> m_impl;

public:

	~IContext() = default;
	explicit operator GLContext&() { return *m_impl; }
};