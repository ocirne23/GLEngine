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

void GLTextureArray::initialize(const rde::vector<Pixmap*>& pixmaps, uint numMipMaps,
	GLint minFilter, GLint magFilter, GLint textureWrapS, GLint textureWrapT)
{
	assert(!pixmaps.empty());

	m_numMipmaps = numMipMaps;

	// If setDimensions was not used before, use the dimensions of the first pixmap;
	if (m_width == 0 || m_height == 0 || m_numComponents == 0)
	{
		m_width = pixmaps[0]->m_width;
		m_height = pixmaps[0]->m_height;
		m_numComponents = pixmaps[0]->m_numComponents;
		m_isFloatTexture = pixmaps[0]->m_isFloatData;
	}

	glGenTextures(1, &m_textureID);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_textureID);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, magFilter);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, textureWrapS);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, textureWrapT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, numMipMaps);

	m_generateMipMaps = (
		minFilter == GL_NEAREST_MIPMAP_LINEAR ||
		minFilter == GL_NEAREST_MIPMAP_NEAREST ||
		minFilter == GL_LINEAR_MIPMAP_LINEAR ||
		minFilter == GL_LINEAR_MIPMAP_NEAREST);

	GLint internalFormat = getInternalFormatForNumComponents(m_numComponents, m_isFloatTexture);
	GLenum format = getFormatForNumComponents(m_numComponents);

	int width = m_width;
	int height = m_height;
	int depth = pixmaps.size();
	for (unsigned int i = 0; i < m_numMipmaps; i++)
	{
		glTexImage3D(GL_TEXTURE_2D_ARRAY, i, internalFormat, width, height, depth, 0,
			format, m_isFloatTexture ? GL_FLOAT : GL_UNSIGNED_BYTE, NULL);
		width = max(1, (int) (width / 2.0f));
		height = max(1, (int) (height / 2.0f));
	}

	for (int i = 0; i < pixmaps.size(); ++i)
	{
		assert(pixmaps[i]->m_width == m_width);
		assert(pixmaps[i]->m_height == m_height);
		assert(pixmaps[i]->m_numComponents == m_numComponents);
		assert(pixmaps[i]->m_isFloatData == m_isFloatTexture);

		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, m_width, m_height, 1, format, m_isFloatTexture ? GL_FLOAT : GL_UNSIGNED_BYTE, pixmaps[i]->m_data.b);
		if (m_generateMipMaps)
			glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
	}

	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	m_initialized = true;
}

void GLTextureArray::setDimensions(uint width, uint height, uint numComponents, bool isFloatTexture)
{
	assert(!m_initialized);
	m_width = width;
	m_height = height;
	m_numComponents = numComponents;
	m_isFloatTexture = isFloatTexture;
}


void GLTextureArray::bind(uint index)
{
	assert(m_initialized);
	glActiveTexture(GL_TEXTURE0 + index);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_textureID);
}

void GLTextureArray::unbind(uint index)
{
	assert(m_initialized);
	glActiveTexture(GL_TEXTURE0 + index);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}