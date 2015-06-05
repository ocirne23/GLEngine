#include "Graphics/GL/Wrappers/GLTextureArray.h"

#include "Graphics/Pixmap.h"
#include "Graphics/GL/GL.h"
#include "Graphics/Pixmap.h"
#include "Graphics/GL/Utils/getGLTextureFormat.h"

#include <assert.h>

BEGIN_UNNAMED_NAMESPACE()
inline int max(int a, int b) { return a > b ? a : b; }
END_UNNAMED_NAMESPACE()

GLTextureArray::~GLTextureArray()
{
	if (m_initialized)
		glDeleteTextures(1, &m_textureID);
}

void GLTextureArray::initialize(const rde::vector<rde::string>& a_filePaths, uint a_numMipMaps,
								ETextureMinFilter a_minFilter, ETextureMagFilter a_magFilter, ETextureWrap a_textureWrapS, ETextureWrap a_textureWrapT)
{
	assert(!a_filePaths.empty());
	rde::vector<Pixmap> pixmaps;
	pixmaps.resize(a_filePaths.size());
	for (int i = 0; i < a_filePaths.size(); ++i)
		pixmaps[i].read(a_filePaths[i].c_str());

	m_numMipmaps = a_numMipMaps;

	// If setDimensions was not used before, use the dimensions of the first pixmap;
	if (m_width == 0 || m_height == 0 || m_numComponents == 0)
	{
		m_width = pixmaps[0].m_width;
		m_height = pixmaps[0].m_height;
		m_numComponents = pixmaps[0].m_numComponents;
		m_isFloatTexture = pixmaps[0].m_isFloatData;
	}

	glGenTextures(1, &m_textureID);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_textureID);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, (GLenum) a_minFilter);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, (GLenum) a_magFilter);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, (GLenum) a_textureWrapS);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, (GLenum) a_textureWrapT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, a_numMipMaps);

	m_generateMipMaps = (
		a_minFilter == ETextureMinFilter::NEAREST_MIPMAP_LINEAR ||
		a_minFilter == ETextureMinFilter::NEAREST_MIPMAP_NEAREST ||
		a_minFilter == ETextureMinFilter::LINEAR_MIPMAP_LINEAR ||
		a_minFilter == ETextureMinFilter::LINEAR_MIPMAP_NEAREST);

	const GLint internalFormat = getInternalFormatForNumComponents(m_numComponents, m_isFloatTexture);
	const GLenum format = getFormatForNumComponents(m_numComponents);
	const GLenum type = m_isFloatTexture ? GL_FLOAT : GL_UNSIGNED_BYTE;

	int width = m_width;
	int height = m_height;
	const int depth = pixmaps.size();
	glTexStorage3D(GL_TEXTURE_2D_ARRAY, a_numMipMaps, internalFormat, m_width, m_height, depth);

	for (int i = 0; i < pixmaps.size(); ++i)
	{
		assert(pixmaps[i].m_width == m_width);
		assert(pixmaps[i].m_height == m_height);
		assert(pixmaps[i].m_numComponents == m_numComponents);
		assert(pixmaps[i].m_isFloatData == m_isFloatTexture);
		// GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *pixels 
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, m_width, m_height, 1, format, type, pixmaps[i].m_data.b);
		//glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, internalFormat, m_width, m_height, depth, 0, format, type, pixmaps[i].m_data.b);
	}
	if (m_generateMipMaps)
		glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	m_initialized = true;
}

void GLTextureArray::setDimensions(uint a_width, uint a_height, uint a_numComponents, bool a_isFloatTexture)
{
	assert(!m_initialized);
	m_width = a_width;
	m_height = a_height;
	m_numComponents = a_numComponents;
	m_isFloatTexture = a_isFloatTexture;
}

void GLTextureArray::bind(uint a_index)
{
	assert(m_initialized);
	glActiveTexture(GL_TEXTURE0 + a_index);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_textureID);
}

void GLTextureArray::unbind(uint a_index)
{
	assert(m_initialized);
	glActiveTexture(GL_TEXTURE0 + a_index);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}