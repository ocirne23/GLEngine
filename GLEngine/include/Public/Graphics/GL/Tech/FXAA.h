#pragma once

#include "Graphics/GL/Wrappers/GLShader.h"
#include "Graphics/GL/Wrappers/GLFramebuffer.h"

class FXAA
{
public:

	enum class EQuality { FASTEST, DEFAULT, HIGH, EXTREME };

	FXAA() {}
	FXAA(const FXAA& copy) = delete;

	void initialize(EQuality quality, uint screenWidth, uint screenHeight);
	void begin();
	void endAndRender();

private:

	GLShader m_fxaaShader;
	GLFramebuffer m_fxaaFBO;
};