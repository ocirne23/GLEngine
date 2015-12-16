#pragma once

#include "Core.h"

class GLCubeMap
{
public:

	GLCubeMap() {}
	GLCubeMap(const GLCubeMap& copy) = delete;
	
	void initialize(uint width, uint height);

	uint getWidth() const     { return m_width; }
	uint getHeight() const    { return m_height; }
	uint getTextureID()       { return m_textureID; }

private:

	bool m_initialized = false;
	uint m_textureID   = 0;
	uint m_width       = 0;
	uint m_height      = 0;
};