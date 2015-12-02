#include "Graphics/GL/Utils/QuadDrawUtils.h"

#include "Core.h"
#include "Graphics/GL/GL.h"

#include <assert.h>

GLShader QuadDrawUtils::s_quadShader;
GLVertexBuffer QuadDrawUtils::s_vertexBuffer;
GLVertexBuffer QuadDrawUtils::s_indiceBuffer;
GLStateBuffer QuadDrawUtils::s_stateBuffer;

void QuadDrawUtils::drawQuad()
{
	if (!s_vertexBuffer.isInitialized())
		initBuffers();
	if (!s_quadShader.isInitialized())
		initQuadShader();

	s_quadShader.begin();
	s_stateBuffer.begin();
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, NULL);
	s_stateBuffer.end();
	s_quadShader.end();
}

void QuadDrawUtils::drawQuad(GLShader& a_shader)
{
	if (!s_vertexBuffer.isInitialized())
		initBuffers();

	assert(a_shader.isInitialized());
	s_stateBuffer.begin();
	if (!a_shader.isBegun())
	{
		a_shader.begin();
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, NULL);
		a_shader.end();
	}
	else
	{
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, NULL);
	}
	s_stateBuffer.end();
}

void QuadDrawUtils::initBuffers()
{
	float quad[] =
	{ // Position				Texcoords
		-1.0f, -1.0f, 0.0f,		0.0f, 0.0f,
		1.0f, -1.0f, 0.0f,		1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,		1.0f, 1.0f,
		-1.0f, 1.0f, 0.0f,		0.0f, 1.0f
	};

	byte indices[] =
	{
		0, 1, 2,
		0, 2, 3
	};

	VertexAttribute attributes[] =
	{
		{0, "POSITION", VertexAttribute::EFormat::FLOAT, 3},
		{1, "TEXCOORDS", VertexAttribute::EFormat::FLOAT, 2}
	};

	s_stateBuffer.initialize();
	s_stateBuffer.begin();

	s_vertexBuffer.initialize(GLVertexBuffer::EBufferType::ARRAY, GLVertexBuffer::EDrawUsage::STATIC);
	s_vertexBuffer.setVertexAttributes(ARRAY_SIZE(attributes), attributes);
	s_vertexBuffer.upload(sizeof(quad), quad);

	s_indiceBuffer.initialize(GLVertexBuffer::EBufferType::ELEMENT_ARRAY, GLVertexBuffer::EDrawUsage::STATIC);
	s_indiceBuffer.upload(sizeof(indices), indices);

	s_stateBuffer.end();
}

void QuadDrawUtils::initQuadShader()
{
	s_quadShader.initialize("Shaders/quad.vert", "Shaders/quad.frag");
	s_quadShader.begin();
	s_quadShader.setUniform1i("u_tex", 0);
	s_quadShader.end();
}