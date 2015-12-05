#pragma once

#include "Core.h"

#include "Graphics/GL/Wrappers/GLVertexBuffer.h"
#include "Graphics/GL/Wrappers/GLStateBuffer.h"
#include "Graphics/GL/Wrappers/GLShader.h"
#include "Graphics/GL/Wrappers/GLUniform.h"
#include "Graphics/Utils/TextureRegion.h"
#include "EASTL/vector.h"

#include <glm/glm.hpp>

class GLTexture;

class SpriteBatch
{
public:

	void initialize(uint size);
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

	GLShader m_shader;
	GLStateBuffer m_stateBuffer;
	GLVertexBuffer m_vertexBuffer;
	GLVertexBuffer m_indiceBuffer;
	GLUniform<glm::mat4> m_mvpUniform;
	eastl::vector<Vertex> m_vertices;

	GLTexture* m_currentTexture = NULL;
	bool m_initialzied = false;
	bool m_begun       = false;
	uint m_size        = 0;
	uint m_drawCount   = 0;
};