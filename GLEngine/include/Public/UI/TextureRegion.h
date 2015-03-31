#pragma once

class GLTexture;

struct TextureRegion
{
	GLTexture* texture;
	float u, v, u2, v2;
};