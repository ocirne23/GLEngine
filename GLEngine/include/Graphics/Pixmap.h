#pragma once

#include "Core.h"

class FileHandle;

class Pixmap
{
public:
	Pixmap() : m_width(0), m_height(0), m_numComponents(0), m_data() {}
	~Pixmap();

	void read(const FileHandle& handle);
	bool exists() const { return m_data.b != NULL; }
	
public:

	bool m_isFloatData;
	int m_width;
	int m_height;
	int m_numComponents;
	union PixmapData { byte* b; float* f; };
	PixmapData m_data;
};