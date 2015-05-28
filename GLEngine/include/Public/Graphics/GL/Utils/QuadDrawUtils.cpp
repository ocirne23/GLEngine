#include "Graphics/GL/Utils/QuadDrawUtils.h"

#include "Core.h"

BEGIN_UNNAMED_NAMESPACE()

static const char* const QUAD_VERT_SHADER_PATH = "da";
static const char* const QUAD_FRAG_SHADER_PATH = "da";

END_UNNAMED_NAMESPACE()

void QuadDrawUtils::drawQuad()
{
	if (!m_quadShader.isInitialized())
		initShader();
	m_quadShader.begin();
	// draw
	m_quadShader.end();
}

void QuadDrawUtils::drawQuad(GLShader& shader)
{
	assert(shader.isInitialized());
	if (!shader.isBegun())
	{
		shader.begin();
		// draw
		shader.end();
	}
	else
	{
		// draw
	}
}

void QuadDrawUtils::initShader()
{
	m_quadShader.initialize()
}