#include "Graphics/GL/Wrappers/GLCubeMap.h"

#include "Graphics/GL/GL.h"

void GLCubeMap::initialize(uint a_width, uint a_height, uint a_numMipmaps, 
	ETextureMinFilter a_minFilter, ETextureMagFilter a_magFilter, ETextureWrap a_textureWrap)
{
	if (m_initialized)
		glDeleteTextures(1, &m_textureID);

	m_width = a_width;
	m_height = a_height;

	glGenTextures(1, &m_textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureID);

	for (uint i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GLint(a_magFilter));
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GLint(a_minFilter));
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GLint(a_textureWrap));
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GLint(a_textureWrap));
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GLint(a_textureWrap));

	m_initialized = true;
}

void GLCubeMap::bind(uint a_index)
{
	glActiveTexture(GL_TEXTURE0 + a_index);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureID);
}

void GLCubeMap::unbind(uint a_index)
{
	glActiveTexture(GL_TEXTURE0 + a_index);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
