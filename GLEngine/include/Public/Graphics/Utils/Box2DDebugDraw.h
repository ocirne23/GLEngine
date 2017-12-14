#include "Core.h"

#include "Box2D/Common/b2Draw.h"
#include "Graphics/GL/Wrappers/GLShader.h"
#include "Graphics/GL/Wrappers/GLStateBuffer.h"
#include "Graphics/GL/Wrappers/GLVertexBuffer.h"
#include "Graphics/GL/Wrappers/GLUniform.h"
#include "Graphics/Utils/PerspectiveCamera.h"

class Box2DDebugDraw : public b2Draw
{
public:

	~Box2DDebugDraw();

	void initialize();
	void render(PerspectiveCamera& camera);
	void reloadShader();

	virtual void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;
	virtual void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;
	virtual void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color) override;
	virtual void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color) override;
	virtual void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) override;
	virtual void DrawTransform(const b2Transform& xf) override;
	virtual void DrawPoint(const b2Vec2& p, float32 size, const b2Color& color) override;

private:

	bool m_initialized = false;

	PerspectiveCamera m_camera;
	GLShader m_debugDrawShader;
	GLStateBuffer m_vao;
	GLVertexBuffer m_vertexBuffer;
	GLVertexBuffer m_indexBuffer;
	GLUniform<glm::mat4> m_vpMatrixUniform;
	b2Transform m_drawTransform;

	struct Vertex
	{
		glm::vec2 position;
		glm::vec3 color;
	};
	eastl::vector<Vertex> m_vertices;
	eastl::vector<uint32> m_indices;
	uint32 m_indiceCounter;
};