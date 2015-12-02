#include "Graphics/GL/GLSpriteBatch.h"

#include "Graphics/GL/GL.h"
#include "Graphics/GL/GLDefines.h"
#include "Graphics/GL/Wrappers/GLTexture.h"

BEGIN_UNNAMED_NAMESPACE()

static const char* VERT_SHADER_FILE = "Shaders/UI/spritebatch.vert";
static const char* FRAG_SHADER_FILE = "Shaders/UI/spritebatch.frag";

END_UNNAMED_NAMESPACE()

void GLSpriteBatch::initialize(uint a_size)
{
	m_size = a_size;

	VertexAttribute attributes[] =
	{
		VertexAttribute(0, "Position", VertexAttribute::EFormat::FLOAT, 2),
		VertexAttribute(1, "Texcoords", VertexAttribute::EFormat::FLOAT, 2),
	};
	
	eastl::vector<ushort> indices;
	indices.reserve(m_size * 6);
	for (uint i = 0, j = 0; i < m_size * 6; i += 6, j += 4)
	{
		indices.push_back(j + 0);
		indices.push_back(j + 1);
		indices.push_back(j + 2);
		indices.push_back(j + 0);
		indices.push_back(j + 2);
		indices.push_back(j + 3);
	}

	m_shader.initialize(VERT_SHADER_FILE, FRAG_SHADER_FILE);
	m_vertexBuffer.initialize(GLVertexBuffer::EBufferType::ARRAY, GLVertexBuffer::EDrawUsage::STREAM);
	m_indiceBuffer.initialize(GLVertexBuffer::EBufferType::ELEMENT_ARRAY, GLVertexBuffer::EDrawUsage::STATIC);
	m_stateBuffer.initialize();

	m_shader.begin();
	m_mvpUniform.initialize(m_shader, "u_mvp");
	m_shader.end();

	m_stateBuffer.begin();
	m_vertexBuffer.setVertexAttributes(ARRAY_SIZE(attributes), attributes);
	m_indiceBuffer.upload(indices.size() * sizeof(ushort), &indices[0]);
	m_stateBuffer.end();

	m_initialzied = true;
}

void GLSpriteBatch::begin(const glm::mat4& a_mvpMatrix)
{
	assert(!m_begun && m_initialzied);
	m_begun = true;
	m_shader.begin();
	m_mvpUniform.set(a_mvpMatrix);
	m_vertices.clear();
}

void GLSpriteBatch::flush()
{
	assert(m_begun && m_initialzied);

	if (m_drawCount && m_currentTexture)
	{
		m_currentTexture->bind(0);

		m_stateBuffer.begin();
		m_vertexBuffer.upload(m_vertices.size() * sizeof(Vertex), &m_vertices[0]);
		glDrawElements(GL_TRIANGLES, m_drawCount * 6, GL_UNSIGNED_SHORT, NULL);
		m_stateBuffer.end();
		m_drawCount = 0;
	}
}

void GLSpriteBatch::draw(const TextureRegion& a_region, float a_x, float a_y, float a_width, float a_height)
{
	assert(m_begun && m_initialzied);
	if (m_currentTexture != a_region.texture || m_drawCount > m_size)
	{
		flush();
		m_currentTexture = a_region.texture;
	}

	++m_drawCount;

	float x2 = a_x + a_width;
	float y2 = a_y + a_height;

	m_vertices.push_back({ a_x, y2, a_region.u2, a_region.v2 });
	m_vertices.push_back({ x2, y2, a_region.u, a_region.v2});
	m_vertices.push_back({x2, a_y, a_region.u, a_region.v});
	m_vertices.push_back({a_x, a_y, a_region.u2, a_region.v});
}

void GLSpriteBatch::end()
{
	assert(m_begun && m_initialzied);
	flush();
	m_shader.end();
	m_begun = false;
}