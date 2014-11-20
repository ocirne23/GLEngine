#pragma once

#include "Graphics/GL/GLTypes.h"

class GLShader;

template <typename T>
class GLUniform
{
public:
	GLUniform() {};
	~GLUniform() {};
	GLUniform(const GLUniform& copy) = delete;

	void initialize(const GLShader& shader, const char* uniformName);
	void set(T value);
	T get() { return m_value; }

private:

	GLint m_uniformLoc;
	T m_value;
};