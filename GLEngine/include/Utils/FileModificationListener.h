#pragma once

#include "rde\rde_string.h"

#include <functional>

class FileHandle;

class FileModificationListener
{
public:
	friend class FileModificationManager;

private:

	FileModificationListener(const FileHandle& handle, std::function<void()> onFileModification);
	~FileModificationListener() {};

	rde::string m_filePath;

	std::function<void()> m_onFileModification;
	struct WriteTime
	{
		unsigned long dwLowDateTime;
		unsigned long dwHighDateTime;
	} m_lastWriteTime;
};