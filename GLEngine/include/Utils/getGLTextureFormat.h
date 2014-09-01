#pragma once

#include "Core.h"
#include "Graphics\GL\GLTypes.h"
#include "Graphics\GL\GLDefines.h"

GLint getInternalFormatForNumComponents(uint numComponents, bool isFloatTexture);

GLenum getFormatForNumComponents(uint numComponents);