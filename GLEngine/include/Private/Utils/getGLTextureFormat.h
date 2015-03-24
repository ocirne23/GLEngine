#pragma once

#include "Core.h"
#include "Graphics/GL/GLTypes.h"
#include "Graphics/GL/GLDefines.h"

GLint getInternalFormatForNumComponents(uint a_numComponents, bool a_isFloatTexture);
GLenum getFormatForNumComponents(uint numComponents);