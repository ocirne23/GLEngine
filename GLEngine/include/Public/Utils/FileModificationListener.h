#pragma once

#include "EASTL/string.h"

#include <functional>

class FileModificationListener
{
public:

	friend class FileModificationManager;

private:

	FileModificationListener(const eastl::string& filePath, std::function<void()> onFileModification)
		: m_filePath(filePath), m_onFileModification(onFileModification)
	{}
	~FileModificationListener() {};

private:

	struct WriteTime
	{
		unsigned long dwLowDateTime = 0;
		unsigned long dwHighDateTime = 0;
	};

private:

	eastl::string m_filePath;
	std::function<void()> m_onFileModification;
	WriteTime m_lastWriteTime;
};