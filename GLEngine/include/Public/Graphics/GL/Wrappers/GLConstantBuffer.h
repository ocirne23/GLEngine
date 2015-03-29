#pragma once

#include "Core.h"

class GLShader;

class GLConstantBuffer
{
public:
	friend class GLStateBuffer;

	enum class EDrawUsage
	{
		STATIC = 0x88E4, // GL_STATIC_DRAW,
		DYNAMIC = 0x88E8, // GL_DYNAMIC_DRAW,
		STREAM = 0x88E0  // GL_STREAM_DRAW
	};

public:

	GLConstantBuffer() {}
	~GLConstantBuffer();
	GLConstantBuffer(const GLConstantBuffer& copy) = delete;

	void initialize(const GLShader& shader, uint bindingPoint, const char* blockName, EDrawUsage drawUsage);
	void upload(uint numBytes, const void* data);
	void bind();

private:

	bool m_initialized       = false;
	const GLShader* m_shader = NULL;
	EDrawUsage m_drawUsage   = EDrawUsage::DYNAMIC;
	uint m_ubo               = 0;
	uint m_bindingPoint      = 0;
	uint m_uboIndex          = 0;
};