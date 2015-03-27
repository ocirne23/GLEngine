#pragma once

#include "Core.h"

#include "Graphics/GL/Wrappers/GLVertexBuffer.h"
#include "Graphics/GL/Wrappers/GLStateBuffer.h"
#include "Graphics/GL/Wrappers/GLShader.h"
#include "Graphics/GL/Wrappers/GLUniform.h"
#include "rde/vector.h"

#include <glm/glm.hpp>

class GLTexture;

class SpriteBatch
{
public:

	SpriteBatch();
	~SpriteBatch();

	void setProjectionMatrix(const glm::mat4& matrix);
	void begin();
	void draw(const TextureRegion& region, float x, float y, float width, float height);
	void drawRotated(const TextureRegion& region, float x, float y, float width, float height, float rotationDeg);
	void end();

private:

	struct Vertex
	{
		float x, y, u, v;
	};

private:

	bool m_begun     = false;
	uint m_size      = 0;
	uint m_drawCount = 0;

	GLTexture* m_currentTexture = NULL;

	GLShader m_shader;
	GLUniform m_mvpUniform;

	GLStateBuffer m_stateBuffer;
	GLVertexBuffer m_vertexBuffer;
	GLVertexBuffer m_indiceBuffer;

	rde::vector<Vertex> m_vertices;
};