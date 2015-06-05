#pragma once

#include "Core.h"

#include "Utils/VecForward.h"

class Pixmap
{
public:
	Pixmap() {}
	~Pixmap() { SAFE_DELETE_ARRAY(m_data.b); }

	void read(const char* filePath);
	bool exists() const { return m_data.b != NULL; }

	void set(uint width, uint height, uint numComp, const glm::vec4& col);

public:

	union PixmapData { byte* b = NULL; float* f; };

public:

	bool m_isFloatData  = false;
	int m_width         = 0;
	int m_height        = 0;
	int m_numComponents = 0;
	PixmapData m_data;
};