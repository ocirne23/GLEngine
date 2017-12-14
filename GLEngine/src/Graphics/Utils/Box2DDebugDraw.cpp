#include "Graphics/Utils/Box2DDebugDraw.h"

#include "Graphics/GL/GL.h"

#include <glm/gtc/constants.hpp>


BEGIN_UNNAMED_NAMESPACE()

const char* BOX2D_DEBUG_VERT_SHADER = "../EngineAssets/Shaders/Box2D/debug.vert";
const char* BOX2D_DEBUG_FRAG_SHADER = "../EngineAssets/Shaders/Box2D/debug.frag";

END_UNNAMED_NAMESPACE()

Box2DDebugDraw::~Box2DDebugDraw()
{
	if (m_initialized)
	{

	}
}

void Box2DDebugDraw::initialize()
{
	m_initialized = true;
	m_debugDrawShader.initialize(BOX2D_DEBUG_VERT_SHADER, BOX2D_DEBUG_FRAG_SHADER);
	m_vao.initialize();
	m_vao.begin();
	m_vertexBuffer.initialize(GLVertexBuffer::EBufferType::ARRAY, GLVertexBuffer::EDrawUsage::STREAM);
	VertexAttribute attributes[] = {
		VertexAttribute(0, VertexAttribute::EFormat::FLOAT, 2),
		VertexAttribute(1, VertexAttribute::EFormat::FLOAT, 3)
	};
	m_vertexBuffer.setVertexAttributes(attributes);
	m_indexBuffer.initialize(GLVertexBuffer::EBufferType::ELEMENT_ARRAY, GLVertexBuffer::EDrawUsage::STREAM);
	m_vao.end();
	m_debugDrawShader.begin();
	m_vpMatrixUniform.initialize(m_debugDrawShader, "u_vpMatrix");
	m_debugDrawShader.end();
}

void Box2DDebugDraw::render(PerspectiveCamera& a_camera)
{
	m_debugDrawShader.begin();
	m_vpMatrixUniform.set(a_camera.getCombinedMatrix());
	m_vao.begin();
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //wireframe rendering

	m_vertexBuffer.upload({ rcast<byte*>(m_vertices.begin()), scast<int64>(m_vertices.size_bytes()) });
	m_indexBuffer.upload({ rcast<byte*>(m_indices.begin()), scast<int64>(m_indices.size_bytes()) });
	glDrawElements(GL_LINES, scast<GLsizei>(m_indices.size() - 1), GL_UNSIGNED_INT, NULL);
	m_vao.end();
	m_debugDrawShader.end();

	m_vertices.clear();
	m_indices.clear();
	m_indiceCounter = 0;
}

void Box2DDebugDraw::reloadShader()
{
	m_debugDrawShader.initialize(BOX2D_DEBUG_VERT_SHADER, BOX2D_DEBUG_FRAG_SHADER);
}

void Box2DDebugDraw::DrawPolygon(const b2Vec2* a_vertices, int32 a_vertexCount, const b2Color& a_color)
{
	const glm::vec2* vertices = rcast<const glm::vec2*>(a_vertices);
	const glm::vec3 color = glm::vec3(a_color.r, a_color.g, a_color.b);
	for (int i = 0; i < a_vertexCount; ++i)
	{
		m_indices.push_back(m_indiceCounter + i);
		m_indices.push_back(m_indiceCounter + i + (i < a_vertexCount - 1 ? 1 : 0));
		m_vertices.push_back({ vertices[i], color });
	}
	m_indiceCounter += a_vertexCount;
}

void Box2DDebugDraw::DrawSolidPolygon(const b2Vec2* a_vertices, int32 a_vertexCount, const b2Color& a_color)
{
	DrawPolygon(a_vertices, a_vertexCount, a_color);
}

void Box2DDebugDraw::DrawCircle(const b2Vec2& a_center, float32 a_radius, const b2Color& a_color)
{
	const int32 segments = glm::max(int32(5.0f * a_radius), 10);
	const float angle = 2 * glm::pi<float>() / segments;
	const float cos = glm::cos(angle);
	const float sin = glm::sin(angle);
	float cx = a_radius;
	float cy = 0;

	owner<b2Vec2*> vertices = new b2Vec2[segments + 1];
	for (int i = 0; i < segments; i++) {
		vertices[i] = a_center + b2Vec2(cx, cy);
		float temp = cx;
		cx = cos * cx - sin * cy;
		cy = sin * temp + cos * cy;
	}
	vertices[segments] = a_center + b2Vec2(cx, cy);
	DrawPolygon(vertices, segments + 1, a_color);
	SAFE_DELETE_ARRAY(vertices);
}

void Box2DDebugDraw::DrawSolidCircle(const b2Vec2& a_center, float32 a_radius, const b2Vec2& a_axis, const b2Color& a_color)
{
	DrawCircle(a_center, a_radius, a_color);
}

void Box2DDebugDraw::DrawSegment(const b2Vec2& a_p1, const b2Vec2& a_p2, const b2Color& a_color)
{
	const b2Vec2 vertices[] = { a_p1, a_p2 };
	DrawSolidPolygon(vertices, ARRAY_SIZE(vertices), a_color);
}

void Box2DDebugDraw::DrawTransform(const b2Transform& a_xf)
{
	m_drawTransform = a_xf;
}

void Box2DDebugDraw::DrawPoint(const b2Vec2& a_p, float32 a_size, const b2Color& a_color)
{
	DrawSolidCircle(a_p, a_size, b2Vec2(0, 0), a_color);
}
