#pragma once

#include "BuilderCore.h"
#include <fstream>

class AssetDatabaseEntry
{
public:
	AssetDatabaseEntry(std::fstream& file, uint size);

	template <typename T>
	void write(T* t);

	template <typename T>
	void read(T* t);

private:
	std::fstream& m_file;
	uint m_written = 0;
	uint m_size;
};