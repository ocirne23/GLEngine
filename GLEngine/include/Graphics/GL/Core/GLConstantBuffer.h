#pragma once

#include "Core.h"
#include "Graphics\GL\GLTypes.h"

class GLShader;

class GLConstantBuffer
{
public:
	friend class GLStateBuffer;

	GLConstantBuffer() : m_initialized(false), m_drawUsage(0), m_ubo(0), m_bindingPoint(0), m_uboIndex(0) {}
	~GLConstantBuffer();
	GLConstantBuffer(const GLConstantBuffer& copy) = delete;

	void initialize(const GLShader& shader, GLuint bindingPoint, const char* blockName, GLenum drawUsage);
	void upload(uint numBytes, const void* data);

	void bind();

private:

	bool m_initialized;

	GLenum m_drawUsage;
	GLuint m_ubo;
	GLuint m_bindingPoint;
	GLuint m_uboIndex;
};