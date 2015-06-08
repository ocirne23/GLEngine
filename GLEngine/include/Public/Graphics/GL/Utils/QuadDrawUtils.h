#pragma once

#include "Graphics/GL/Wrappers/GLVertexBuffer.h"
#include "Graphics/GL/Wrappers/GLStateBuffer.h"
#include "Graphics/GL/Wrappers/GLShader.h"

class QuadDrawUtils
{
public:

	static void drawQuad();
	static void drawQuad(GLShader& shader);

private:

	QuadDrawUtils() {}

	static void initBuffers();
	static void initQuadShader();

private:

	static GLShader s_quadShader;
	static GLVertexBuffer s_vertexBuffer;
	static GLVertexBuffer s_indiceBuffer;
	static GLStateBuffer s_stateBuffer;
};