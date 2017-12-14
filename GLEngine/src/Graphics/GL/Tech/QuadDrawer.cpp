#include "Graphics/GL/Tech/QuadDrawer.h"

#include "Core.h"
#include "Graphics/GL/GL.h"

#include <assert.h>

BEGIN_UNNAMED_NAMESPACE()

const char* const QUAD_VERT_SHADER_PATH = "../EngineAssets/Shaders/quad.vert";
const char* const QUAD_FRAG_SHADER_PATH = "../EngineAssets/Shaders/quad.frag";

END_UNNAMED_NAMESPACE()

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
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL);
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
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL);
		a_shader.end();
	}
	else
	{
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL);
	}
	s_stateBuffer.end();
}

void QuadDrawer::initBuffers()
{
	const float quad[] =
	{// Position				Texcoords
		-1.0f, -1.0f, 0.0f,		0.0f, 0.0f,
		1.0f, -1.0f, 0.0f,		1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,		1.0f, 1.0f,
		-1.0f, 1.0f, 0.0f,		0.0f, 1.0f
	};

	const short indices[] =
	{
		0, 1, 2,
		0, 2, 3
	};

	const VertexAttribute attributes[] =
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
	s_quadShader.initialize(QUAD_VERT_SHADER_PATH, QUAD_FRAG_SHADER_PATH);
	s_quadShader.begin();
	s_quadShader.setUniform1i("u_tex", 0);
	s_quadShader.end();
}