#include "Graphics/Pixmap.h"

#include "Utils/EResourceType.h"
#include "Utils/FileHandle.h"

#include <glm/glm.hpp>

#include <assert.h>

BEGIN_UNNAMED_NAMESPACE()

byte floatToByteCol(float f)
{
	float f2 = glm::max(0.0f, glm::min(1.0f, f));
	return (byte) floor(f2 == 1.0f ? 255 : f2 * 256.0f);
}

END_UNNAMED_NAMESPACE()

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
		file.readBytes(reinterpret_cast<char*>(m_data.f), m_width * m_height * m_numComponents * sizeof(float), sizeof(uint) * 4);
	}
}

void Pixmap::set(uint a_width, uint a_height, uint a_numComp, bool a_isFloatTexture, void* a_data)
{
	assert(!m_data.b);
	m_width = a_width;
	m_height = a_height;
	m_numComponents = a_numComp;
	m_isFloatData = a_isFloatTexture;
	m_data.b = (byte*) a_data;
}


void Pixmap::set(uint a_width, uint a_height, uint a_numComp, const glm::vec4& a_col)
{
	assert(!m_data.b);
	const byte byteCol[] = { floatToByteCol(a_col.r), floatToByteCol(a_col.g), floatToByteCol(a_col.b), floatToByteCol(a_col.b)};
	m_width = a_width;
	m_height = a_height;
	m_isFloatData = false;
	m_numComponents = a_numComp;
	m_data.b = new byte[m_width * m_height * m_numComponents];
	for (uint x = 0; x < a_width; ++x)
	{
		for (uint y = 0; y < a_height; ++y)
		{
			memcpy(m_data.b + (x * y + y) * a_numComp, byteCol, a_numComp);
		}
	}
}
