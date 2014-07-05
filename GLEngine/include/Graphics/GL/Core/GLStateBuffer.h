#pragma once

#include "Graphics\GL\GLTypes.h"
#include "rde\vector.h"

class GLConstantBuffer;
class GLTextureBuffer;
class GLVertexBuffer;

/** VAO wrapper */
class GLStateBuffer
{
public:
	GLStateBuffer() : m_initialized(false), m_isBegun(false), m_vao(0) {}
	~GLStateBuffer();
	GLStateBuffer(const GLStateBuffer& copy) = delete;

	void initialize();
	void begin();
	void end();

	static bool isBegun() { return s_isBegun; }

private:

	static bool s_isBegun;

	bool m_initialized;
	bool m_isBegun;
	GLuint m_vao;
};