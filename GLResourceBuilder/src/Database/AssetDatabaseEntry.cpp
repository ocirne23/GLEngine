#include "Database/AssetDatabaseEntry.h"

AssetDatabaseEntry::AssetDatabaseEntry(std::fstream& a_file, uint a_size) : m_file(a_file), m_size(a_size)
{
	
}

template <typename T>
void AssetDatabaseEntry::write(T* t)
{
	uint size = sizeof(T);
	m_written += size;
	if (m_written <= m_size)
	{
		m_file.write(reinterpret_cast<const char*>(t), size);
	}
	else
	{
		assert(false);
	}
}

template <typename T>
void AssetDatabaseEntry::read(T* t)
{

}