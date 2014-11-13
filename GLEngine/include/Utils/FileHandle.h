#pragma once

#include "Core.h"

struct SDL_RWops;

#include "rde/rde_string.h"

class FileHandle
{
public:

	enum FileMode { FileMode_READ, FileMode_WRITE, FileMode_READWRITE };

	FileHandle(const rde::string& filePath, FileMode fileMode = FileMode_READ);
	FileHandle(const char* path, FileMode fileMode = FileMode_READ);
	~FileHandle();

	void close();

	void readBytes(char* buffer, uint numBytes, uint offset) const;
	void writeBytes(const char* bytes, uint numBytes);

	rde::string readString(uint numChars) const;
	rde::string readString() const;

	const rde::string& getFilePath() const	{ return m_filePath; }
	uint getFileSize() const				{ return m_size; }
	bool exists() const						{ return m_rwops != NULL; }

private:

	void initialize(const char* path, FileMode fileMode);

private:

	rde::string m_filePath;
	FileMode m_fileMode;
	uint m_size;
	bool m_isOpen;

	SDL_RWops* m_rwops;
};