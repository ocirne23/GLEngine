#pragma once

#include "Core.h"
#include "GraphicsAPI.h"

enum class EContextType
{
	OPENGL
};

class IBuffer;
enum class EBufferUsageType;

class GRAPHICS_API IContext
{
public:

	virtual EContextType    getContextType() const = 0;
	virtual owner<IBuffer*> createBuffer(uint64 size, const EBufferUsageType& type) = 0;
	virtual void            destroyBuffer(owner<IBuffer*>& buffer) = 0;

protected:

	virtual ~IContext() {}
};