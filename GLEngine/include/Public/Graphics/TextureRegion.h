#pragma once

#include "Core.h"

class GLTexture;

struct TextureRegion
{
	GLTexture* texture = NULL;
	float u  = 1.0f;
	float v  = 0.0f;
	float u2 = 0.0f;
	float v2 = 1.0f;
};