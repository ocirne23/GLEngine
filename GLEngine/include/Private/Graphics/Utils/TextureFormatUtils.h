#pragma once

#include "Core.h"
#include "Graphics/GL/GLTypes.h"
#include "Graphics/GL/GLDefines.h"

class TextureFormatUtils
{
public:

	static GLint getInternalFormatForNumComponents(uint a_numComponents, bool a_isFloatTexture);
	static GLenum getFormatForNumComponents(uint numComponents);
	static uint getNumComponentsForFormat(GLenum format);

private:

	TextureFormatUtils() {}
};

