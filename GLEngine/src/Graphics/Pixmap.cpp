#include "Graphics\Pixmap.h"

#include "Utils\FileHandle.h"

#include "Utils\stb_image.h"

void Pixmap::readRaw(const char* fileName)
{
	FileHandle file(fileName);
	assert(file.exists());
	if (!file.exists())
		print("Pixmap does not exist %s \n", fileName);

	m_data = new byte[file.getFileSize()];
	file.readBytes(reinterpret_cast<char*>(&m_width), sizeof(uint));
	file.readBytes(reinterpret_cast<char*>(&m_height), sizeof(uint));
	file.readBytes(reinterpret_cast<char*>(&m_numComponents), sizeof(uint));
	file.readBytes(reinterpret_cast<char*>(m_data), m_width * m_height * m_numComponents);
	file.close();
}

Pixmap::~Pixmap()
{
	if (m_data)
		delete[] m_data;
}