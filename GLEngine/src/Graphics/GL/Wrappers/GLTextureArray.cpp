#include "Graphics/GL/Wrappers/GLTextureArray.h"

#include "Graphics/GL/GL.h"
#include "Graphics/Utils/TextureFormatUtils.h"
#include "Database/Assets/DBTexture.h"
#include "EASTL/vector.h"

#include <assert.h>

GLTextureArray::~GLTextureArray()
{
	if (m_initialized)
		glDeleteTextures(1, &m_textureID);
}

void GLTextureArray::startInit(uint a_width, uint a_height, uint a_depth, uint a_numComponents, bool a_isFloatTexture, uint a_numMipMaps, 
                               ETextureMinFilter a_minFilter, ETextureMagFilter a_magFilter, ETextureWrap a_textureWrapS, ETextureWrap a_textureWrapT)
{
	if (m_initialized)
		glDeleteTextures(1, &m_textureID);
	m_initialized = false;

	m_width = a_width;
	m_height = a_height;
	m_depth = a_depth;
	m_numComponents = a_numComponents;
	m_isFloatTexture = a_isFloatTexture;
	m_numMipmaps = a_numMipMaps;

	const bool generateMipMaps = (
		a_minFilter == ETextureMinFilter::NEAREST_MIPMAP_LINEAR ||
		a_minFilter == ETextureMinFilter::NEAREST_MIPMAP_NEAREST ||
		a_minFilter == ETextureMinFilter::LINEAR_MIPMAP_LINEAR ||
		a_minFilter == ETextureMinFilter::LINEAR_MIPMAP_NEAREST);

	if (!generateMipMaps)
		m_numMipmaps = 0;

	glGenTextures(1, &m_textureID);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_textureID);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, (GLenum) a_minFilter);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, (GLenum) a_magFilter);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, (GLenum) a_textureWrapS);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, (GLenum) a_textureWrapT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, (GLint) m_numMipmaps);

	const GLint internalFormat = TextureFormatUtils::getInternalFormatForNumComponents(m_numComponents, m_isFloatTexture);
	glTexStorage3D(GL_TEXTURE_2D_ARRAY, m_numMipmaps + 1, internalFormat, m_width, m_height, m_depth);
}

uint GLTextureArray::addTexture(const DBTexture& a_tex)
{
	assert(m_width != 0 && m_height != 0 && "Use startInit before addTexture");
	// Every texture in the array must have the same size and format
	assert(a_tex.getWidth() == m_width);
	assert(a_tex.getHeight() == m_height);
	assert(a_tex.getNumComponents() == m_numComponents);
	if (a_tex.getFormat() == DBTexture::EFormat::FLOAT)
		assert(m_isFloatTexture);
	if (a_tex.getFormat() == DBTexture::EFormat::BYTE)
		assert(!m_isFloatTexture);
	// Cannot add more textures than depth
	assert(m_numTexturesAdded < m_depth);

	const GLenum format = TextureFormatUtils::getFormatForNumComponents(m_numComponents);
	const GLenum type = m_isFloatTexture ? GL_FLOAT : GL_UNSIGNED_BYTE;

	glBindTexture(GL_TEXTURE_2D_ARRAY, m_textureID);
	glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, m_numTexturesAdded, m_width, m_height, 1, format, type, (const GLvoid*) &a_tex.getData()[0]);

	return m_numTexturesAdded++;
}

void GLTextureArray::finishInit()
{
	assert(!m_initialized);
	assert(m_width != 0 && m_height != 0 && "Use startInit and addTexture before finishInit");
	assert(m_numTexturesAdded == m_depth && "Texture array is not filled");

	glBindTexture(GL_TEXTURE_2D_ARRAY, m_textureID);
	if (m_numMipmaps)
		glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	m_initialized = true;
}

void GLTextureArray::bind(uint a_index)
{
	assert(m_initialized);
	glActiveTexture(GL_TEXTURE0 + a_index);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_textureID);
}

void GLTextureArray::unbind(uint a_index)
{
	glActiveTexture(GL_TEXTURE0 + a_index);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}



