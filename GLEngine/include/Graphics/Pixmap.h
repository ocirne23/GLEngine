#pragma once

#include "Core.h"

class FileHandle;

class Pixmap
{
public:
	Pixmap() {}
	~Pixmap() {	SAFE_DELETE_ARRAY(m_data.b); }

	void read(const FileHandle& handle);
	bool exists() const { return m_data.b != NULL; }
	
public:

	union PixmapData { byte* b = NULL; float* f; };

public:

	bool m_isFloatData	= false;
	int m_width			= 0;
	int m_height		= 0;
	int m_numComponents = 0;
	PixmapData m_data;
};