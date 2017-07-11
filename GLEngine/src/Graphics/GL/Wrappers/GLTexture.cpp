#include "Graphics/GL/Wrappers/GLTexture.h"

#include "Database/Assets/DBTexture.h"
#include "Graphics/GL/GL.h"
#include "Graphics/Utils/CheckGLError.h"
#include "Graphics/Utils/TextureFormatUtils.h"

GLTexture::~GLTexture()
{
	if (m_initialized)
		glDeleteTextures(1, &m_textureID);
}

void GLTexture::bind(uint a_index)
{
	glActiveTexture(GL_TEXTURE0 + a_index);
	glBindTexture(GL_TEXTURE_2D, m_textureID);
}

void GLTexture::unbind(uint a_index)
{
	glActiveTexture(GL_TEXTURE0 + a_index);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void GLTexture::initialize(ESizedFormat a_format, uint a_width, uint a_height, 
                           EMultiSampleType a_multiSampleType, ETextureCompareMode a_compareMode,
                           ETextureMinFilter a_minFilter, ETextureMagFilter a_magFilter, 
                           ETextureWrap a_textureWrapS, ETextureWrap a_textureWrapT)
{
	GLenum textureType;
	m_width = a_width;
	m_height = a_height;
	m_numComponents = TextureFormatUtils::getNumComponentsForFormat(GLenum(a_format));
	m_multiSampleType = a_multiSampleType;

	glGenTextures(1, &m_textureID);
	if (a_multiSampleType == EMultiSampleType::NONE)
	{
		textureType = GL_TEXTURE_2D;
		glBindTexture(textureType, m_textureID);
		glTexStorage2D(textureType, 1, GLenum(a_format), a_width, a_height);
		glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, GLenum(a_minFilter));
		glTexParameteri(textureType, GL_TEXTURE_MAG_FILTER, GLenum(a_magFilter));
		glTexParameteri(textureType, GL_TEXTURE_WRAP_S, GLenum(a_textureWrapS));
		glTexParameteri(textureType, GL_TEXTURE_WRAP_T, GLenum(a_textureWrapT));
	}
	else
	{
		textureType = GL_TEXTURE_2D_MULTISAMPLE;
		glBindTexture(textureType, m_textureID);
		glTexStorage2DMultisample(textureType, GLsizei(a_multiSampleType), GLenum(a_format), a_width, a_height, GL_TRUE);
	}
	if (a_compareMode != ETextureCompareMode::NONE)
	{	// AMD bug? GL_TEXTURE_2D_MULTISAMPLE doesnt work with glTexParameteri?
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GLint(a_compareMode));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
	}
	CHECK_GL_ERROR();

	glBindTexture(textureType, 0);
}

void GLTexture::initialize(const DBTexture& a_texture, uint a_numMipmaps,
                           ETextureMinFilter a_minFilter, ETextureMagFilter a_magFilter, 
                           ETextureWrap a_textureWrapS, ETextureWrap a_textureWrapT)
{
	if (m_initialized)
		glDeleteTextures(1, &m_textureID);

	m_width = a_texture.getWidth();
	m_height = a_texture.getHeight();
	m_numComponents = a_texture.getNumComponents();
	const byte* data = &a_texture.getData()[0];

	const bool isFloatTexture = (a_texture.getFormat() == DBTexture::EFormat::FLOAT);
	const GLint internalFormat = TextureFormatUtils::getInternalFormatForNumComponents(m_numComponents, isFloatTexture);
	const GLint format = TextureFormatUtils::getFormatForNumComponents(m_numComponents);
	const GLenum type = isFloatTexture ? GL_FLOAT : GL_UNSIGNED_BYTE;
	const bool generateMipMaps = (
		a_minFilter == ETextureMinFilter::NEAREST_MIPMAP_LINEAR ||
		a_minFilter == ETextureMinFilter::NEAREST_MIPMAP_NEAREST ||
		a_minFilter == ETextureMinFilter::LINEAR_MIPMAP_LINEAR ||
		a_minFilter == ETextureMinFilter::LINEAR_MIPMAP_NEAREST);
	if (!generateMipMaps)
		a_numMipmaps = 0;

	glGenTextures(1, &m_textureID);
	glBindTexture(GL_TEXTURE_2D, m_textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GLenum(a_minFilter));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GLenum(a_magFilter));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GLenum(a_textureWrapS));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GLenum(a_textureWrapT));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, a_numMipmaps);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_width, m_height, 0, format, type, scast<const GLvoid*>(data));

	if (a_numMipmaps)
		glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

	bool m_initialized = true;
}