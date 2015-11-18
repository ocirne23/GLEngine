#include "Utils/FileHandle.h"

#include "Core.h"
#include "3rdparty/rde/vector.h"
#include "3rdparty/rde/rde_string.h"

#include <SDL/SDL_rwops.h>

#include <conio.h>
#include <stdlib.h>
#include <direct.h>

const rde::string FileHandle::ASSETS_DIR("assets/");

FileHandle::FileHandle(const rde::string& a_filePath, EFileMode a_fileMode) : m_rwops(0), m_size(0), m_fileMode(a_fileMode)
{
	initialize(a_filePath.c_str(), a_fileMode);
}

FileHandle::FileHandle(const char* a_filePath, EFileMode a_fileMode) : m_rwops(0), m_size(0), m_fileMode(a_fileMode)
{
	initialize(a_filePath, a_fileMode);
}

void FileHandle::initialize(const char* a_filePath, EFileMode a_fileMode)
{
	rde::string dir(ASSETS_DIR);
	dir.append(a_filePath);
	a_filePath = dir.c_str();

	char fullPath[_MAX_PATH];
	if (!_fullpath(fullPath, a_filePath, _MAX_PATH))
		print("Could not construct full path %s \n", a_filePath);
	
	m_filePath = fullPath;

	const char* fileModeStr;
	switch (a_fileMode)
	{
	case EFileMode::READ:       fileModeStr = "rb";	break;
	case EFileMode::WRITE:      fileModeStr = "wb";	break;
	case EFileMode::READWRITE:  fileModeStr = "w+b"; break;
	default:                    return;
	}

	m_rwops = SDL_RWFromFile(a_filePath, fileModeStr);
	m_isOpen = (m_rwops != NULL);
	if (!m_rwops)
	{
		print("Could not find or open the file %s \n", a_filePath);
		return;
	}

	m_size = (uint64) SDL_RWsize(m_rwops);
}

FileHandle::~FileHandle()
{
	if (m_isOpen)
		close();
}

void FileHandle::readBytes(char* a_buffer, uint64 a_numBytes, uint a_offset) const
{
	assert(m_isOpen);
	assert(a_numBytes + a_offset <= m_size);
	assert(m_fileMode == EFileMode::READ || m_fileMode == EFileMode::READWRITE);

	SDL_RWseek(m_rwops, a_offset, RW_SEEK_SET);
	uint64 read = SDL_RWread(m_rwops, a_buffer, a_numBytes, 1);
}

void FileHandle::writeBytes(const char* a_bytes, uint64 a_numBytes)
{
	assert(m_isOpen);
	assert(m_fileMode == EFileMode::WRITE || m_fileMode == EFileMode::READWRITE);

	SDL_RWseek(m_rwops, 0, RW_SEEK_END);
	SDL_RWwrite(m_rwops, a_bytes, a_numBytes, 1);
	m_size += a_numBytes;
}

rde::string FileHandle::readString(uint64 a_numChars) const
{
	char* chars = new char[a_numChars];
	readBytes(chars, a_numChars, 0);
	rde::string str(chars, (int) a_numChars);
	delete [] chars;
	return str;
}

rde::string FileHandle::readString() const
{
	return readString(m_size);
}

void FileHandle::close()
{
	assert(m_isOpen);
	m_isOpen = false;
	SDL_RWclose(m_rwops);
}