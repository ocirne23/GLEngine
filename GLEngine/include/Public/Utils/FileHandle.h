#pragma once

#include "Core.h"

#include "3rdparty/rde/rde_string.h"

struct SDL_RWops;

class FileHandle
{
public:

	enum class EFileMode { READ, WRITE, READWRITE };

	FileHandle(const rde::string& filePath, EFileMode fileMode = EFileMode::READ);
	FileHandle(const char* path, EFileMode fileMode = EFileMode::READ);
	~FileHandle();

	void close();
	void readBytes(char* buffer, uint numBytes, uint offset) const;
	void writeBytes(const char* bytes, uint numBytes);

	rde::string readString(uint64 numChars) const;
	rde::string readString() const;

	const rde::string& getFilePath() const { return m_filePath; }
	uint64 getFileSize() const             { return m_size; }
	bool exists() const                    { return m_rwops != NULL; }

public:

	static const rde::string ASSETS_DIR;

private:

	void initialize(const char* path, EFileMode fileMode);

private:

	rde::string m_filePath;
	EFileMode m_fileMode = EFileMode::READ;
	uint64 m_size       = 0;
	bool m_isOpen       = false;
	SDL_RWops* m_rwops  = NULL;
};