#pragma once

#include "Core.h"
#include <glm/glm.hpp>

class GLCubeMap
{
public:

	enum class ETextureMinFilter
	{
		NEAREST                = 0x2600, // GL_NEAREST
		LINEAR                 = 0x2601, // GL_LINEAR
		NEAREST_MIPMAP_NEAREST = 0x2700, // GL_NEAREST_MIPMAP_NEAREST
		LINEAR_MIPMAP_NEAREST  = 0x2701, // GL_LINEAR_MIPMAP_NEAREST
		NEAREST_MIPMAP_LINEAR  = 0x2702, // GL_NEAREST_MIPMAP_LINEAR
		LINEAR_MIPMAP_LINEAR   = 0x2703, // GL_LINEAR_MIPMAP_LINEAR
	};

	enum class ETextureMagFilter
	{
		NEAREST = 0x2600, // GL_NEAREST
		LINEAR  = 0x2601, // GL_LINEAR
	};

	enum class ETextureWrap
	{
		CLAMP_TO_EDGE   = 0x812F, // GL_CLAMP_TO_EDGE
		MIRRORED_REPEAT = 0x8370, // GL_MIRRORED_REPEAT
		REPEAT          = 0x2901, // GL_REPEAT
	};

public:

	GLCubeMap() {}
	GLCubeMap(const GLCubeMap& copy) = delete;
	
	void initialize(uint width, uint height, uint numMipmaps = 0,
		ETextureMinFilter minFilter = ETextureMinFilter::NEAREST,
		ETextureMagFilter magFilter = ETextureMagFilter::NEAREST,
		ETextureWrap textureWrap = ETextureWrap::CLAMP_TO_EDGE);
	void bind(uint index = 0);
	void unbind(uint index = 0);

	uint getWidth() const     { return m_width; }
	uint getHeight() const    { return m_height; }
	uint getTextureID() const { return m_textureID; }

	void setPosition(const glm::vec3& a_position) { m_position = a_position; }
	const glm::vec3& getPosition() const { return m_position; }

private:

	bool m_initialized = false;
	uint m_textureID   = 0;
	uint m_width       = 0;
	uint m_height      = 0;
	glm::vec3 m_position;
};