#pragma once

#include "Graphics/GL/Wrappers/GLShader.h"

class QuadDrawUtils
{
public:

	static void drawQuad();
	static void drawQuad(GLShader& shader);

private:

	QuadDrawUtils() {}
	static void initShader();

private:

	static GLShader m_quadShader;
};