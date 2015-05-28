#pragma once

#include "Graphics/GL/Wrappers/GLShader.h"

class QuadDrawUtils
{
public:

	void drawQuad();
	void drawQuad(GLShader& shader);

private:

	QuadDrawUtils() {}
	void initShader();

private:

	GLShader m_quadShader;
};