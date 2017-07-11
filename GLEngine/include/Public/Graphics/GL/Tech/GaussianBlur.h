#pragma once

#include "Graphics/GL/Wrappers/GLFramebuffer.h"
#include "Graphics/GL/Wrappers/GLShader.h"
#include "Graphics/GL/Wrappers/GLTexture.h"

class GaussianBlur
{
public:

	enum class EBlurValueType { FLOAT, VEC2, VEC3, VEC4 };

	GaussianBlur() {}
	GaussianBlur(const GaussianBlur& copy) = delete;

	void initialize(EBlurValueType type, uint xRes, uint yRes);
	// Blurs the first texture in the blurredFBO, uses the depth texture of the depthFBO for the bilateral blur.
	// blurredFBO and depthFBO can be identical
	void blurFBO(GLFramebuffer& blurredFBO);
	void reloadShader();

private:

	bool m_initialized    = false;
	float m_pixelXOffset  = 0.0;
	float m_pixelYOffset  = 0.0;
	EBlurValueType m_type = EBlurValueType::VEC3;
	GLTexture::ESizedFormat m_format = GLTexture::ESizedFormat::RGB8;

	GLShader m_blurShader;
	GLTexture m_blurResultTexture;
	GLFramebuffer m_blurXResultFBO;
};