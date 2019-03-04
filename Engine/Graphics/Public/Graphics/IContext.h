#pragma once

#include "Core.h"
#include "GraphicsAPI.h"

enum class EContextType
{
	OPENGL
};

class IBuffer;
enum class EBufferUsageType;
class ITexture;
enum class ETextureType;

class GRAPHICS_API IContext
{
public:

	virtual EContextType     getContextType() const = 0;
	virtual owner<IBuffer*>  createBuffer(uint64 size, const EBufferUsageType& type) = 0;
	virtual void             destroyBuffer(owner<IBuffer*>& buffer) = 0;
	virtual owner<ITexture*> createTexture(const ETextureType& type) = 0;
	virtual void             destroyTexture(owner<ITexture*> texture) = 0;

protected:

	virtual ~IContext() {}
};