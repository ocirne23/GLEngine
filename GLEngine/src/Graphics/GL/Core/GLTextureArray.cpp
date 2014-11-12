#include "Graphics\GL\Core\GLTextureArray.h"

#include "Graphics\Pixmap.h"
#include "Graphics\GL\GL.h"
#include "Utils\getGLTextureFormat.h"

#include <assert.h>

inline int max(int a, int b) { return a > b ? a : b; }

GLTextureArray::~GLTextureArray()
{
	if (m_initialized)
		glDeleteTextures(1, &m_textureID);
}

void GLTextureArray::initialize(const rde::vector<Pixmap*>& a_pixmaps, uint a_numMipMaps,
	GLint a_minFilter, GLint a_magFilter, GLint a_textureWrapS, GLint a_textureWrapT)
{
	assert(!a_pixmaps.empty());

	m_numMipmaps = a_numMipMaps;

	// If setDimensions was not used before, use the dimensions of the first pixmap;
	if (m_width == 0 || m_height == 0 || m_numComponents == 0)
	{
		m_width = a_pixmaps[0]->m_width;
		m_height = a_pixmaps[0]->m_height;
		m_numComponents = a_pixmaps[0]->m_numComponents;
		m_isFloatTexture = a_pixmaps[0]->m_isFloatData;
	}

	glGenTextures(1, &m_textureID);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_textureID);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, a_minFilter);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, a_magFilter);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, a_textureWrapS);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, a_textureWrapT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, a_numMipMaps);

	m_generateMipMaps = (
		a_minFilter == GL_NEAREST_MIPMAP_LINEAR ||
		a_minFilter == GL_NEAREST_MIPMAP_NEAREST ||
		a_minFilter == GL_LINEAR_MIPMAP_LINEAR ||
		a_minFilter == GL_LINEAR_MIPMAP_NEAREST);

	GLint internalFormat = getInternalFormatForNumComponents(m_numComponents, m_isFloatTexture);
	GLenum format = getFormatForNumComponents(m_numComponents);

	int width = m_width;
	int height = m_height;
	int depth = a_pixmaps.size();
	for (uint i = 0; i < m_numMipmaps; i++)
	{
		glTexImage3D(GL_TEXTURE_2D_ARRAY, i, internalFormat, width, height, depth, 0,
			format, m_isFloatTexture ? GL_FLOAT : GL_UNSIGNED_BYTE, NULL);
		width = max(1, (int) (width / 2.0f));
		height = max(1, (int) (height / 2.0f));
	}

	for (int i = 0; i < a_pixmaps.size(); ++i)
	{
		assert(a_pixmaps[i]->m_width == m_width);
		assert(a_pixmaps[i]->m_height == m_height);
		assert(a_pixmaps[i]->m_numComponents == m_numComponents);
		assert(a_pixmaps[i]->m_isFloatData == m_isFloatTexture);

		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, m_width, m_height, 1, format, m_isFloatTexture ? GL_FLOAT : GL_UNSIGNED_BYTE, a_pixmaps[i]->m_data.b);
		if (m_generateMipMaps)
			glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
	}

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