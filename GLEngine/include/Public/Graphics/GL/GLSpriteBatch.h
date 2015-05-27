#pragma once

#include "Core.h"

#include "Graphics/GL/Wrappers/GLVertexBuffer.h"
#include "Graphics/GL/Wrappers/GLStateBuffer.h"
#include "Graphics/GL/Wrappers/GLShader.h"
#include "Graphics/GL/Wrappers/GLUniform.h"
#include "3rdparty/rde/vector.h"

#include "UI/TextureRegion.h"

#include <glm/glm.hpp>

class GLTexture;

class GLSpriteBatch
{
public:

	GLSpriteBatch(uint size);

	void begin(const glm::mat4& mvpMatrix);
	void draw(const TextureRegion& region, float x, float y, float width, float height);
	void end();

private:

	void flush();

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
	GLUniform<glm::mat4> m_mvpUniform;

	GLStateBuffer m_stateBuffer;
	GLVertexBuffer m_vertexBuffer;
	GLVertexBuffer m_indiceBuffer;

	rde::vector<Vertex> m_vertices;
};