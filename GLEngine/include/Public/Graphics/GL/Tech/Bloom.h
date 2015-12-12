#pragma once

#include "Graphics/GL/Wrappers/GLFramebuffer.h"
#include "Graphics/GL/Wrappers/GLShader.h"
#include "Graphics/GL/Tech/GaussianBlur.h"

class Bloom
{
public:

	Bloom() {}
	Bloom(const Bloom& copy) = delete;

	void initialize(uint xRes, uint yRes);
	GLFramebuffer& getBloomResultFBO(GLFramebuffer& sceneFBO);

private:

	bool m_initialized = false;

	GLShader m_bloomShader;
	GLFramebuffer m_bloomResultFBO;
	GaussianBlur m_gaussianBlur;
};