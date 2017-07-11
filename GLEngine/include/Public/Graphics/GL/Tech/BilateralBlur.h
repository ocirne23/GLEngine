#pragma once

#include "Graphics/GL/Wrappers/GLFramebuffer.h"
#include "Graphics/GL/Wrappers/GLShader.h"

class BilateralBlur
{
public:

	enum class EBlurValueType { FLOAT, VEC2, VEC3, VEC4 };

	BilateralBlur() {}
	BilateralBlur(const BilateralBlur& copy) = delete;

	void initialize(EBlurValueType type, uint xRes, uint yRes, uint blurRadius, uint blurTexWidth, uint blurTexHeight);
	// Blurs the first texture in the blurredFBO, uses the depth texture of the depthFBO for the bilateral blur.
	// blurredFBO and depthFBO can be identical
	void blurFBO(GLFramebuffer& blurredFBO, GLFramebuffer& depthFBO);
	void reloadShader();

private:

	bool m_initialized    = false;
	float m_blurRadius    = 0.0f;
	float m_sharpness     = 1000.0f;
	uint m_xRes           = 0;
	uint m_yRes           = 0;
	uint m_blurTexWidth   = 0;
	uint m_blurTexHeight  = 0;
	EBlurValueType m_type = EBlurValueType::VEC3;
	GLTexture::ESizedFormat m_format = GLTexture::ESizedFormat::RGB8;

	GLShader m_blurXShader;
	GLShader m_blurYShader;
	GLTexture m_blurResultTexture;
	GLFramebuffer m_blurXResultFBO;
};