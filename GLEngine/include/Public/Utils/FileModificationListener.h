#pragma once

#include "Core.h"
#include "EASTL/string.h"
#include "EASTL/vector.h"

#include <functional>
#include <memory>

class FileModificationListener
{
public:

	friend class FileModificationManager;

private:

	FileModificationListener(const eastl::string& filePath);
	~FileModificationListener() {};

	void update();
	void addCallback(std::weak_ptr<std::function<void()>> onFileModification);
	uint numCallbacks() const { return uint(m_callbackList.size()); }

private:

	struct WriteTime
	{
		unsigned long dwLowDateTime = 0;
		unsigned long dwHighDateTime = 0;
	};

private:

	eastl::string m_fullFilePath;
	eastl::vector<std::weak_ptr<std::function<void()>>> m_callbackList;
	WriteTime m_lastWriteTime;
};