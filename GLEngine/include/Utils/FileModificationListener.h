#pragma once

#include "rde/rde_string.h"

#include <functional>

class FileModificationListener
{
public:

	friend class FileModificationManager;

private:

	FileModificationListener(const rde::string& filePath, std::function<void()> onFileModification);
	~FileModificationListener() {};

private:

	struct WriteTime
	{
		unsigned long dwLowDateTime = 0;
		unsigned long dwHighDateTime = 0;
	};

private:

	rde::string m_filePath;
	std::function<void()> m_onFileModification;
	WriteTime m_lastWriteTime;
};