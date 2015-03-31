#pragma once

#include "Core.h"

class GLShader;

class GLTextureBuffer
{
public:
	friend class GLStateBuffer;

	enum class EDrawUsage
	{
		STATIC  = 0x88E4, // GL_STATIC_DRAW,
		DYNAMIC = 0x88E8, // GL_DYNAMIC_DRAW,
		STREAM  = 0x88E0  // GL_STREAM_DRAW
	};

	enum class ESizedFormat
	{
		R8       = 0x8229, // GL_R8,
		R16	     = 0x822A, // GL_R16,
		R16F	 = 0x822D, // GL_R16F,
		R32F	 = 0x822E, // GL_R32F,
		R8I	     = 0x8231, // GL_R8I,
		R16I	 = 0x8233, // GL_R16I,
		R32I	 = 0x8235, // GL_R32I,
		R8UI	 = 0x8232, // GL_R8UI,
		R16UI	 = 0x8234, // GL_R16UI,
		R32UI	 = 0x8236, // GL_R32UI,
		RG8	     = 0x822B, // GL_RG8,
		RG16	 = 0x822C, // GL_RG16,
		RG16F	 = 0x822F, // GL_RG16F,
		RG32F	 = 0x8230, // GL_RG32F,
		RG8I	 = 0x8237, // GL_RG8I,
		RG16I	 = 0x8239, // GL_RG16I,
		RG32I	 = 0x823B, // GL_RG32I,
		RG8UI    = 0x8238, // GL_RG8UI,
		RG16UI	 = 0x823A, // GL_RG16UI,
		RG32UI	 = 0x823C, // GL_RG32UI,
		RGBA8	 = 0x8058, // GL_RGBA8,
		RGBA16	 = 0x805B, // GL_RGBA16,
		RGBA16F  = 0x881A, // GL_RGBA16F
		RGBA32F  = 0x8814, // GL_RGBA32F
		RGBA8I	 = 0x8D8E, // GL_RGBA8I,
		RGBA16I  = 0x8D88, // GL_RGBA16I
		RGBA32I  = 0x8D82, // GL_RGBA32I
		RGBA8UI  = 0x8D7C, // GL_RGBA8UI
		RGBA16UI = 0x8D76, // GL_RGBA16UI
		RGBA32UI = 0x8D70  // GL_RGBA32UI
	};

public:

	GLTextureBuffer() {}
	~GLTextureBuffer();
	GLTextureBuffer(const GLTextureBuffer& copy) = delete;

	void initialize(const GLShader& shader, const char* samplerName, int textureIdx, ESizedFormat sizedFormat, EDrawUsage drawUsage = EDrawUsage::STATIC);
	void upload(uint numBytes, const void* data);
	void bind();

private:

	bool m_initialized = false;
	int m_textureLoc   = 0;
	int m_textureIdx   = 0;
	uint m_textureID   = 0;
	uint m_bufferID    = 0;

	const GLShader* m_shader = NULL;

	EDrawUsage m_drawUsage             = EDrawUsage::STATIC;
	ESizedFormat m_sizedInternalFormat = ESizedFormat::R8UI;
};