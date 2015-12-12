#pragma once

#include "Core.h"
#include "EASTL/vector.h"

class GLFramebuffer
{
public:

	// https://www.opengl.org/sdk/docs/man/html/glTexStorage2D.xhtml

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

		RGB8     = 0x8051, // GL_RGB8,
		RGB16    = 0x8054, // GL_RGB16
		RGB16F   = 0x881B, // GL_RGB16F
		RGB32F   = 0x8815, // GL_RGB32F
		RGB8I    = 0x8D8F, // GL_RGB8I
		RGB16I   = 0x8D89, // GL_RGB16I
		RGB32I   = 0x8D83, // GL_RGB32I
		RGB8UI   = 0x8D7D, // GL_RGB8UI
		RGB16UI  = 0x8D77, // GL_RGB16UI
		RGB32UI  = 0x8D71, // GL_RGB32UI

		RGBA8    = 0x8058, // GL_RGBA8,
		RGBA16   = 0x805B, // GL_RGBA16,
		RGBA16F  = 0x881A, // GL_RGBA16F
		RGBA32F  = 0x8814, // GL_RGBA32F
		RGBA8I   = 0x8D8E, // GL_RGBA8I,
		RGBA16I  = 0x8D88, // GL_RGBA16I
		RGBA32I  = 0x8D82, // GL_RGBA32I
		RGBA8UI  = 0x8D7C, // GL_RGBA8UI
		RGBA16UI = 0x8D76, // GL_RGBA16UI
		RGBA32UI = 0x8D70, // GL_RGBA32UI

		DEPTH16  = 0x81A5, // GL_DEPTH_COMPONENT16
		DEPTH24  = 0x81A6, // GL_DEPTH_COMPONENT24
		DEPTH32  = 0x81A7  // GL_DEPTH_COMPONENT32
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

	enum class EMultiSampleType
	{
		NONE = 0,
		MSAA_2X = 2,
		MSAA_4X = 4,
		MSAA_8X = 8,
		MSAA_16X = 16
	};

public:

	GLFramebuffer() {};
	~GLFramebuffer();
	GLFramebuffer(const GLFramebuffer& copy) = delete;

	void initialize(EMultiSampleType multiSampleType = EMultiSampleType::NONE);

	void addFramebufferTexture(ESizedFormat format, EAttachment attachment, uint width, uint height);
	void setDepthbufferTexture(ESizedFormat format, uint width, uint height);

	void end();
	void begin();

	void bindDepthTexture(uint textureUnit = 0);
	void bindTexture(uint textureIndex, uint textureUnit);
	void unbindTexture(uint textureIndex, uint textureUnit);
	void deleteTextures();

	bool isMultisampleEnabled() const { return m_multiSampleType == EMultiSampleType::NONE; }

private:

	bool m_initialized  = false;
	bool m_begun        = false;
	uint m_fbo          = 0;
	uint m_depthTexture = 0;
	EMultiSampleType m_multiSampleType = EMultiSampleType::NONE;
	eastl::vector<uint> m_textures;
	eastl::vector<EAttachment> m_drawBuffers;

private:

	static bool s_begun;
};