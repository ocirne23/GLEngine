#pragma once

#include "Core.h"

#include <glm/fwd.hpp>

class IBuffer;
enum class EBufferType;

class IFramebuffer;

enum class EContextType
{
	OPENGL
};

class IContext
{
public:
	
	virtual EContextType getType() const = 0;

	virtual void setViewportSize(uint width, uint height) = 0;
	virtual void clearColor(const glm::vec4& color, bool clearDepth) = 0;
	
	virtual owner<IBuffer*> createBuffer(const EBufferType& type) = 0;
	virtual void destroyBuffer(owner<IBuffer*> buffer) = 0;

	virtual owner<IFramebuffer*> createFramebuffer() = 0;
	virtual void destroyFramebuffer(owner<IFramebuffer*> framebuffer) = 0;

protected:

	friend class IWindow;
	virtual ~IContext() {}
};