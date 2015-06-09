#pragma once

#include "Graphics/GL/Wrappers/GLFramebuffer.h"
#include "Graphics/GL/Wrappers/GLShader.h"
#include "Graphics/GL/Wrappers/GLTexture.h"

class PerspectiveCamera;

class HBAO
{
public:
	HBAO() {}
	~HBAO();

	void initialize(const PerspectiveCamera& camera);

	void begin();
	void endAndRender();

private:

	bool m_isInitialized;

	GLFramebuffer m_fboFullRes;
	GLFramebuffer m_blurXFbo;
	GLFramebuffer m_blurYFbo;
	GLShader m_hbaoFullShader;
	GLShader m_blurXShader;
	GLShader m_blurYShader;
	GLTexture m_noiseTexture;
};