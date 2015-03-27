#include "UI/SpriteBatch.h"

BEGIN_UNNAMED_NAMESPACE()

END_UNNAMED_NAMESPACE()

SpriteBatch::SpriteBatch()
{
	m_vertexBuffer.initialize(GL_ARRAY_BUFFER, GL_STREAM_DRAW);
	m_indiceBuffer.initialize(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW);
	m_shader.initialize()
}