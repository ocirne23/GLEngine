#include "Utils\FileHandle.h"

#include "Core.h"
#include "rde\vector.h"
#include "rde\rde_string.h"

#include <SDL\SDL_rwops.h>

#include <conio.h>
#include <stdlib.h>
#include <direct.h>

FileHandle::FileHandle(const rde::string& filePath, FileMode fileMode) : m_rwops(0), m_size(0), m_fileMode(fileMode)
{
	initialize(filePath.c_str(), fileMode);
}

FileHandle::FileHandle(const char* filePath, FileMode fileMode) : m_rwops(0), m_size(0), m_fileMode(fileMode)
{
	initialize(filePath, fileMode);
}

void FileHandle::initialize(const char* filePath, FileMode fileMode)
{
#ifndef ANDROID
	rde::string dir("assets/");
	dir.append(filePath);
	filePath = dir.c_str();

	char fullPath[_MAX_PATH];
	if (!_fullpath(fullPath, filePath, _MAX_PATH))
		print("Could not construct full path %s \n", filePath);
	m_filePath = fullPath;

#endif

	const char* fileModeStr;
	switch (fileMode)
	{
	case FileMode_READ:				fileModeStr = "rb";	break;
	case FileMode_WRITE:			fileModeStr = "wb";	break;
	case FileMode_READWRITE:		fileModeStr = "w+b"; break;
	default:						fileModeStr = "w+b"; break;
	}

	m_rwops = SDL_RWFromFile(filePath, fileModeStr);
	m_isOpen = (m_rwops != NULL);
	if (!m_rwops)
	{
		print("Could not find or open the file %s \n", filePath);
		return;
	}

	m_size = (uint) SDL_RWsize(m_rwops);
}

FileHandle::~FileHandle()
{
	if (m_isOpen)
		close();
}

void FileHandle::readBytes(char* buffer, uint numBytes, uint offset) const
{
	assert(m_isOpen);
	assert(numBytes + offset <= m_size);
	assert(m_fileMode == FileMode_READ || m_fileMode == FileMode_READWRITE);

	SDL_RWseek(m_rwops, offset, RW_SEEK_SET);
	SDL_RWread(m_rwops, buffer, numBytes, 1);
}

void FileHandle::writeBytes(const char* bytes, uint numBytes)
{
	assert(m_isOpen);
	assert(m_fileMode == FileMode_WRITE || m_fileMode == FileMode_READWRITE);

	SDL_RWseek(m_rwops, 0, RW_SEEK_END);
	SDL_RWwrite(m_rwops, bytes, numBytes, 1);
	m_size += numBytes;
}

rde::string FileHandle::readString(uint numChars) const
{
	char* chars = new char[numChars];
	readBytes(chars, numChars, 0);
	rde::string str(chars, numChars);
	delete chars;
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