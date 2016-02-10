#pragma once

#include "Core.h"
#include "EASTL/string.h"

class EUBOBindingPoint;
class GLShader;

class GLConstantBuffer
{
public:
	friend class GLStateBuffer;

	enum class EDrawUsage
	{
		STATIC = 0x88E4,  // GL_STATIC_DRAW,
		DYNAMIC = 0x88E8, // GL_DYNAMIC_DRAW,
		STREAM = 0x88E0   // GL_STREAM_DRAW
	};

	struct Config
	{
		uint bindingPoint;
		eastl::string name;
		EDrawUsage drawUsage;
		uint dataSize;
	};

public:

	GLConstantBuffer() {}
	~GLConstantBuffer();
	GLConstantBuffer(const GLConstantBuffer& copy);

	void initialize(const Config& config);
	void initialize(uint bindingPoint, const char* blockName, EDrawUsage drawUsage, uint dataSize);
	void upload(uint numBytes, const void* data, uint offset = 0);
	byte* mapBuffer();
	void unmapBuffer();

	void bind();
	bool isInitialized() const { return m_initialized; }

private:

	bool m_initialized     = false;
	bool m_isMapped        = false;
	EDrawUsage m_drawUsage = EDrawUsage::DYNAMIC;
	uint m_ubo             = 0;
	uint m_bindingPoint    = 0;
};