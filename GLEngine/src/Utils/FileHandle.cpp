#include "Utils\FileHandle.h"

#include "Core.h"
#include "rde\vector.h"
#include "rde\rde_string.h"

#include <SDL\SDL_rwops.h>

FileHandle::FileHandle(const char* path, FileMode fileMode) : m_pointer(0), m_rwops(0), m_size(0), m_closed(false), m_fileMode(fileMode)
{
#ifndef ANDROID
	rde::string dir("assets/");
	dir.append(path);
	path = dir.c_str();
#endif

	const char* fileModeStr;
	switch (fileMode)
	{
	case FileMode_READ:			fileModeStr = "rb";	break;
	case FileMode_WRITE:			fileModeStr = "wb";	break;
	case FileMode_READWRITE:		fileModeStr = "w+b"; break;
	default:						fileModeStr = "w+b"; break;   
	}

	m_rwops = SDL_RWFromFile(path, fileModeStr);
	if (!m_rwops)
		print("Could not find or open the file %s \n", path);

	m_size = (uint) SDL_RWsize(m_rwops);
}

FileHandle::~FileHandle()
{
	if (!m_closed)
		close();
}

void FileHandle::readBytes(char* buffer, uint numBytes)
{
	assert(!m_closed);
	assert(m_fileMode == FileMode_READ || m_fileMode == FileMode_READWRITE);
	m_pointer += numBytes;
	assert(m_pointer <= m_size);
	SDL_RWread(m_rwops, buffer, numBytes, 1);
}

void FileHandle::writeBytes(char* bytes, uint numBytes)
{
	assert(!m_closed);
	assert(m_fileMode == FileMode_WRITE || m_fileMode == FileMode_READWRITE);
	SDL_RWwrite(m_rwops, bytes, numBytes, 1);
}

rde::string FileHandle::readString(uint numChars)
{
	rde::vector<byte>* buffer = new rde::vector<byte>(numChars);
	readBytes(reinterpret_cast<char*>(buffer->begin()), numChars);
	rde::string str(reinterpret_cast<char*>(buffer->begin()), buffer->size());
	delete buffer;
	return str;
}

void FileHandle::close()
{
	assert(!m_closed);
	m_closed = true;
	SDL_RWclose(m_rwops);
}