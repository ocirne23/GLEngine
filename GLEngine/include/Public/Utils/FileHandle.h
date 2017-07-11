#pragma once

#include "Core.h"

#include "EASTL/string.h"

struct SDL_RWops;

class FileHandle
{
public:

	enum class EFileMode { READ, WRITE, READWRITE };

	FileHandle(const eastl::string& filePath, EFileMode fileMode = EFileMode::READ);
	FileHandle(const char* path, EFileMode fileMode = EFileMode::READ);
	~FileHandle();

	void close();
	void readBytes(char* buffer, uint64 numBytes, uint offset) const;
	void writeBytes(const char* bytes, uint64 numBytes);

	eastl::string readString(uint64 numChars) const;
	eastl::string readString() const;

	const eastl::string& getFilePath() const { return m_filePath; }
	uint64 getFileSize() const               { return m_size; }
	bool exists() const                      { return m_rwops != NULL; }

public:

	static const eastl::string ASSETS_DIR;

private:

	void initialize(const char* path, EFileMode fileMode);

private:

	eastl::string m_filePath;
	EFileMode m_fileMode = EFileMode::READ;
	uint64 m_size        = 0;
	bool m_isOpen        = false;
	SDL_RWops* m_rwops   = NULL;
};