#pragma once

#include "Core.h"
#include "EASTL/vector.h"
#include "EASTL/string.h"
#include <assert.h>
#include <fstream>

class AssetDatabaseEntry
{
public:
	AssetDatabaseEntry::AssetDatabaseEntry(std::fstream& a_file, uint64 a_filePos, uint64 a_size)
		: m_file(a_file), m_totalSize(a_size), m_filePos(a_filePos)
	{}
	
	uint64 getTotalSize() const    { return m_totalSize; }
	uint64 getFileStartPos() const { return m_filePos; }
	bool validateWritten() const   { return m_numBytesWritten == m_totalSize; }
	bool validateRead() const      { return m_numBytesRead == m_totalSize; }

	// Writeops
	template <typename T>
	void writeVal(const T& a_val)
	{
		m_file.seekp(m_filePos + m_numBytesWritten);
		uint size = sizeof(T);
		if (m_numBytesWritten + size <= m_totalSize)
		{
			m_file.write(reinterpret_cast<const char*>(&a_val), size);
			m_numBytesWritten += size;
		}
		else
		{
			print("Max: %i, current: %i, size: %i, overwritten: %i\n", m_totalSize, m_numBytesWritten, size, (m_numBytesWritten + size) - m_totalSize);
			assert(false);
		}
	}

	template <typename T>
	void writeArray(const T* a_val, uint a_length)
	{
		writeVal(a_length);
		uint size = sizeof(T) * a_length;
		if (a_length)
		{
			if (m_numBytesWritten + size <= m_totalSize)
			{
				m_file.write(reinterpret_cast<const char*>(a_val), size);
				m_numBytesWritten += size;
			}
			else
			{
				print("Max: %i, current: %i, size: %i, overwritten: %i\n", m_totalSize, m_numBytesWritten, size, (m_numBytesWritten + size) - m_totalSize);
				assert(false);
			}
		}
	}

	template <typename T>
	void writeVector(const eastl::vector<T>& a_vector)
	{
		uint numElements = (uint) a_vector.size();
		writeVal(numElements);
		if (numElements)
		{
			uint64 byteSize = numElements * sizeof(T);
			if (m_numBytesWritten + byteSize <= m_totalSize)
			{
				m_file.write(reinterpret_cast<const char*>(&a_vector[0]), byteSize);
				m_numBytesWritten += byteSize;
			}
			else
			{
				print("Max: %i, current: %i, size: %i, overwritten: %i\n", m_totalSize, m_numBytesWritten, byteSize, (m_numBytesWritten + byteSize) - m_totalSize);
				assert(false);
			}
		}
	}

	void writeString(const eastl::string& a_str)
	{
		uint strlen = (uint) a_str.length();
		writeVal(strlen);
		if (strlen)
		{
			if (m_numBytesWritten + strlen <= m_totalSize)
			{
				m_file.write(a_str.c_str(), strlen);
				m_numBytesWritten += strlen;
			}
			else
			{
				print("Max: %i, current: %i, size: %i, overwritten: %i\n", m_totalSize, m_numBytesWritten, strlen, (m_numBytesWritten + strlen) - m_totalSize);
				assert(false);
			}
		}
	}

	template <typename T>
	static uint64 getValWriteSize(const T& a_val)
	{
		return sizeof(T);
	}

	template <typename T>
	static uint64 getArrayWriteSize(const T* a_array, uint a_arrayLength)
	{
		return sizeof(uint) + sizeof(T) * a_arrayLength;
	}

	template <typename T>
	static uint64 getVectorWriteSize(const eastl::vector<T>& a_vector)
	{
		return sizeof(uint) + sizeof(T) * a_vector.size();
	}

	static uint64 getStringWriteSize(const eastl::string& a_str)
	{
		return sizeof(uint) + a_str.length();
	}

	// Readops
	template <typename T>
	void readVal(T& a_val)
	{
		m_file.seekg(m_filePos + m_numBytesRead);
		uint size = sizeof(T);
		if (m_numBytesRead + size <= m_totalSize)
		{
			m_file.read(reinterpret_cast<char*>(&a_val), size);
			m_numBytesRead += size;
		}
	}

	template <typename T>
	void readArray(T* a_val, uint* a_outLength = NULL)
	{
		uint length;
		readVal(length);
		uint size = sizeof(T) * length;
		if (m_numBytesRead + size <= m_totalSize && size)
		{
			m_file.read(reinterpret_cast<char*>(a_val), size);
			m_numBytesRead += size;
		}
	}

	template <typename T>
	void readVector(eastl::vector<T>& a_vec)
	{
		uint length;
		readVal(length);
		a_vec.resize(length);
		uint size = sizeof(T) * length;
		if (m_numBytesRead + size <= m_totalSize && length)
		{
			m_file.read(reinterpret_cast<char*>(&a_vec[0]), size);
			m_numBytesRead += size;
		}
	}

	void readString(eastl::string& a_str)
	{
		uint length;
		readVal(length);
		char* buffer = new char[length];
		if (m_numBytesRead + length <= m_totalSize && length)
		{
			m_file.read(reinterpret_cast<char*>(buffer), length);
			a_str.assign(buffer, length);
			m_numBytesRead += length;
		}
		delete[] buffer;
	}

private:

	std::fstream& m_file;
	uint64 m_filePos = 0;
	uint64 m_numBytesWritten = 0;
	uint64 m_totalSize = 0;
	uint64 m_numBytesRead = 0;
};