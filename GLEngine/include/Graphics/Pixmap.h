#pragma once

#include "Core.h"

class Pixmap
{
public:
	Pixmap() : m_width(0), m_height(0), m_numComponents(0), m_data(0) {}
	~Pixmap();

	void readRaw(const char* fileName);
	bool exists() const { return m_data != NULL; }

	static void writeRaw(const char* fileName, const char* destFileName);
	
public:

	int m_width;
	int m_height;
	int m_numComponents;
	byte* m_data;
};