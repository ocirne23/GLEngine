#include "Graphics/GL/Tech/QuadDrawer.h"

#include "Core.h"
#include "Graphics/GL/GL.h"

#include <assert.h>

GLShader QuadDrawer::s_quadShader;
GLVertexBuffer QuadDrawer::s_vertexBuffer;
GLVertexBuffer QuadDrawer::s_indiceBuffer;
GLStateBuffer QuadDrawer::s_stateBuffer;

void QuadDrawer::drawQuad()
{
	if (!s_vertexBuffer.isInitialized())
		initBuffers();
	if (!s_quadShader.isInitialized())
		reloadShader();

	s_quadShader.begin();
	s_stateBuffer.begin();
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, NULL);
	s_stateBuffer.end();
	s_quadShader.end();
}

void QuadDrawer::drawQuad(GLShader& a_shader)
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

void QuadDrawer::initBuffers()
{
	float quad[] =
	{// Position				Texcoords
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
		{0, VertexAttribute::EFormat::FLOAT, 3}, // Position
		{1, VertexAttribute::EFormat::FLOAT, 2}  // Texcoords
	};

	s_stateBuffer.initialize();
	s_stateBuffer.begin();

	s_vertexBuffer.initialize(GLVertexBuffer::EBufferType::ARRAY, GLVertexBuffer::EDrawUsage::STATIC);
	s_vertexBuffer.setVertexAttributes(as_span(attributes, ARRAY_SIZE(attributes)));
	s_vertexBuffer.upload(as_span(rcast<const byte*>(quad), sizeof(quad)));

	s_indiceBuffer.initialize(GLVertexBuffer::EBufferType::ELEMENT_ARRAY, GLVertexBuffer::EDrawUsage::STATIC);
	s_indiceBuffer.upload(as_span(rcast<const byte*>(indices), sizeof(indices)));

	s_stateBuffer.end();
}

void QuadDrawer::reloadShader()
{
	s_quadShader.initialize("Shaders/quad.vert", "Shaders/quad.frag");
	s_quadShader.begin();
	s_quadShader.setUniform1i("u_tex", 0);
	s_quadShader.end();
}