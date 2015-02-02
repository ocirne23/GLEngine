#include "Graphics/Pixmap.h"

#include "Utils/EResourceType.h"
#include "Utils/FileHandle.h"

#include <assert.h>

void Pixmap::read(const FileHandle& a_file)
{
	assert(!m_data.b);

	int type;
	a_file.readBytes(reinterpret_cast<char*>(&type), sizeof(uint), 0);
	a_file.readBytes(reinterpret_cast<char*>(&m_width), sizeof(uint), sizeof(uint));
	a_file.readBytes(reinterpret_cast<char*>(&m_height), sizeof(uint), sizeof(uint) * 2);
	a_file.readBytes(reinterpret_cast<char*>(&m_numComponents), sizeof(uint), sizeof(uint) * 3);

	assert(type == EResourceType_BYTEIMAGE || type == EResourceType_FLOATIMAGE);

	if (type == EResourceType_BYTEIMAGE)
	{
		m_isFloatData = false;
		m_data.b = new byte[m_width * m_height * m_numComponents];
		a_file.readBytes(reinterpret_cast<char*>(m_data.b), m_width * m_height * m_numComponents, sizeof(uint) * 4);
	}
	else
	{
		m_isFloatData = true;
		m_data.f = new float[m_width * m_height * m_numComponents];
		a_file.readBytes(reinterpret_cast<char*>(m_data.f), m_width * m_height * m_numComponents * 4, sizeof(uint) * 4);
	}
}

Pixmap::~Pixmap()
{
	if (m_data.b)
		delete[] ((void*) m_data.b);
}