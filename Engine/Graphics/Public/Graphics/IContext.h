#pragma once

#include "Core.h"
#include "GraphicsAPI.h"

#include <glm/fwd.hpp>

class IBuffer;
enum class EBufferType;

class IFramebuffer;
class IVertexAttributes;
class IShader;
class IVertexShaderStage;
class IFragmentShaderStage;

enum class EContextType
{
	OPENGL
};

class GLContext;

class GRAPHICS_API IContext
{
public:

	IContext();
	IContext(const IContext&) = delete;
	IContext(IContext&& move);
	~IContext();
	explicit operator GLContext&() { return *m_impl; }
	
	//EContextType getType() const;

private:

	owner<GLContext*> m_impl = nullptr;
};