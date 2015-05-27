#pragma once

#include "Core.h"

#include "3rdparty/rde/vector.h"

class GLFramebuffer
{
public:

	enum class ESizedFormat
	{
		R8       = 0x8229, // GL_R8,
		R16      = 0x822A, // GL_R16,
		R16F     = 0x822D, // GL_R16F,
		R32F     = 0x822E, // GL_R32F,
		R8I      = 0x8231, // GL_R8I,
		R16I     = 0x8233, // GL_R16I,
		R32I     = 0x8235, // GL_R32I,
		R8UI     = 0x8232, // GL_R8UI,
		R16UI    = 0x8234, // GL_R16UI,
		R32UI    = 0x8236, // GL_R32UI,
		RG8      = 0x822B, // GL_RG8,
		RG16     = 0x822C, // GL_RG16,
		RG16F    = 0x822F, // GL_RG16F,
		RG32F    = 0x8230, // GL_RG32F,
		RG8I     = 0x8237, // GL_RG8I,
		RG16I    = 0x8239, // GL_RG16I,
		RG32I    = 0x823B, // GL_RG32I,
		RG8UI    = 0x8238, // GL_RG8UI,
		RG16UI   = 0x823A, // GL_RG16UI,
		RG32UI   = 0x823C, // GL_RG32UI,
		RGBA8    = 0x8058, // GL_RGBA8,
		RGBA16   = 0x805B, // GL_RGBA16,
		RGBA16F  = 0x881A, // GL_RGBA16F
		RGBA32F  = 0x8814, // GL_RGBA32F
		RGBA8I   = 0x8D8E, // GL_RGBA8I,
		RGBA16I  = 0x8D88, // GL_RGBA16I
		RGBA32I  = 0x8D82, // GL_RGBA32I
		RGBA8UI  = 0x8D7C, // GL_RGBA8UI
		RGBA16UI = 0x8D76, // GL_RGBA16UI
		RGBA32UI = 0x8D70  // GL_RGBA32UI
	};

	enum class EAttachment
	{
		DEPTH   = 0x8D00, // GL_DEPTH_ATTACHMENT
		STENCIL = 0x8D20, // GL_STENCIL_ATTACHMENT
		COLOR0  = 0x8CE0, // GL_COLOR_ATTACHMENT0
		COLOR1  = 0x8CE1, // GL_COLOR_ATTACHMENT1
		COLOR2  = 0x8CE2, // GL_COLOR_ATTACHMENT2
		COLOR3  = 0x8CE3, // GL_COLOR_ATTACHMENT3
		COLOR4  = 0x8CE4, // GL_COLOR_ATTACHMENT4
		COLOR5  = 0x8CE5  // GL_COLOR_ATTACHMENT5
	};

public:

	GLFramebuffer();
	~GLFramebuffer();
	GLFramebuffer(const GLFramebuffer& copy) = delete;

	void addFramebufferTexture(ESizedFormat format, EAttachment attachment, uint width, uint height);
	void setDepthbufferTexture(ESizedFormat format, uint width, uint height);

	void end();
	void begin();

	void bindTextures(uint fromTextureUnit = 0);
	void bindDepthTexture(uint textureUnit = 0);
	void bindTexture(uint textureIndex, uint textureUnit);
	void deleteTextures();

private:

	bool m_begun        = false;
	uint m_fbo          = 0;
	uint m_depthTexture = 0;

	rde::vector<uint> m_textures;
	rde::vector<EAttachment> m_drawBuffers;

private:

	static bool s_begun;
};