#pragma once

#include "Core.h"

#include "3rdparty/rde/vector.h"

class GLStateBuffer
{
public:
	GLStateBuffer() {}
	~GLStateBuffer();
	GLStateBuffer(const GLStateBuffer& copy) = delete;

	void initialize();
	void begin();
	void end();
	bool isInitialized() const { return m_initialized; }

public:

	static bool isBegun() { return s_isBegun; }

private:

	bool m_initialized = false;
	bool m_isBegun     = false;
	uint m_vao         = 0;

private:

	static bool s_isBegun;
};