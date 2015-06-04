#include "Graphics/Pixmap.h"

#include "Utils/EResourceType.h"
#include "Utils/FileHandle.h"

#include <assert.h>

void Pixmap::read(const char* a_filePath)
{
	FileHandle file(a_filePath, FileHandle::EFileMode::READ);
	assert(file.exists());
	assert(!m_data.b);

	int type;
	file.readBytes(reinterpret_cast<char*>(&type), sizeof(uint), 0);
	file.readBytes(reinterpret_cast<char*>(&m_width), sizeof(uint), sizeof(uint));
	file.readBytes(reinterpret_cast<char*>(&m_height), sizeof(uint), sizeof(uint) * 2);
	file.readBytes(reinterpret_cast<char*>(&m_numComponents), sizeof(uint), sizeof(uint) * 3);

	assert(type == (int) EResourceType::BYTEIMAGE || type == (int) EResourceType::FLOATIMAGE);

	if (type == (int) EResourceType::BYTEIMAGE)
	{
		m_isFloatData = false;
		m_data.b = new byte[m_width * m_height * m_numComponents];
		file.readBytes(reinterpret_cast<char*>(m_data.b), m_width * m_height * m_numComponents, sizeof(uint) * 4);
	}
	else
	{
		m_isFloatData = true;
		m_data.f = new float[m_width * m_height * m_numComponents];
		file.readBytes(reinterpret_cast<char*>(m_data.f), m_width * m_height * m_numComponents * 4, sizeof(uint) * 4);
	}
}