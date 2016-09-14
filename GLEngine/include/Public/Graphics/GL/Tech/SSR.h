#pragma once

#include "Graphics/GL/Wrappers/GLFramebuffer.h"
#include "Graphics/GL/Wrappers/GLShader.h"
#include "Graphics/GL/Wrappers/GLConstantBuffer.h"
#include "Graphics/GL/Wrappers/GLTexture.h"
#include "Graphics/GL/Tech/BilateralBlur.h"

#include <glm/glm.hpp>

class PerspectiveCamera;

class SSR
{
public:

public:

	SSR() {}
	~SSR() {}

	void initialize(const PerspectiveCamera& camera, uint xRes, uint yRes);
	void reloadShader();
	GLFramebuffer& getSSRResultFBO(GLFramebuffer& sceneFBO);

private:

	bool m_initialized = false;

	GLShader m_hbaoFullShader;
	GLFramebuffer m_ssrResultFBO;
	GLTexture m_noiseTexture;
	BilateralBlur m_bilateralBlur;
};