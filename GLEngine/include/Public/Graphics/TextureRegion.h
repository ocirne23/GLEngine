#pragma once

class GLTexture;

struct TextureRegion
{
	GLTexture* texture = NULL;
	float u  = 0.0f;
	float v  = 1.0f;
	float u2 = 0.0f;
	float v2 = 1.0f;
};