#pragma once

#include "Core.h"

#include "rde\rde_string.h"
#include "rde\vector.h"

struct SDL_RWops;

class FileHandle
{
public:
	enum FileMode { FileMode_READ, FileMode_WRITE, FileMode_READWRITE };

	FileHandle(const char* path, FileMode fileMode = FileMode_READ);
	~FileHandle();
	void close();

	void readBytes(char* buffer, uint numBytes);
	rde::string readString(uint numChars);
	inline rde::string readString() { return readString(m_size); }

	void writeBytes(char* bytes, uint numBytes);

	uint getFileSize() { return m_size; }
	bool exists() { return m_rwops != NULL; }

private:

	SDL_RWops* m_rwops;
	uint m_size;
	uint m_pointer;
	bool m_closed;
	FileMode m_fileMode;
};