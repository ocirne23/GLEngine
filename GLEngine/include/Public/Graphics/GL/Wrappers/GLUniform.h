#pragma once

#include "Core.h"

class GLShader;

template <typename T>
class GLUniform
{
public:
	GLUniform() {}
	~GLUniform() {}
	GLUniform(const GLUniform& copy) = delete;

	void initialize(const GLShader& shader, const char* a_uniformName);
	void set(T value);
	T get() { return m_value; }

private:

	const GLShader* m_shader = NULL;
	int m_uniformLoc         = 0;
	T m_value                = (T) 0;
};