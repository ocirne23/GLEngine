#pragma once

#include "Graphics/GL/Wrappers/GLFramebuffer.h"
#include "Graphics/GL/Wrappers/GLShader.h"

class BilateralBlur
{
public:

	enum class EBlurValueType { FLOAT, VEC2, VEC3, VEC4 };

	BilateralBlur() {}
	BilateralBlur(const BilateralBlur& copy) = delete;

	void initialize(EBlurValueType type, uint xRes, uint yRes);
	// Blurs the first texture in the blurredFBO, uses the depth texture of the depthFBO for the bilateral blur.
	// blurredFBO and depthFBO can be identical
	void blurFBO(GLFramebuffer& blurredFBO, GLFramebuffer& depthFBO);

private:

	bool m_initialized = false;

	GLShader m_blurXShader;
	GLShader m_blurYShader;
	GLFramebuffer m_blurXResultFBO;
};