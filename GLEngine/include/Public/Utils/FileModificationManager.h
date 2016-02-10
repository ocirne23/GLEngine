#pragma once

#include "gsl/gsl.h"
#include "EASTL/string.h"
#include "EASTL/hash_map.h"

#include <functional>

class FileModificationListener;

class FileModificationManager
{
public:
	friend class FileModificationListener;

	static void update();
	static void createModificationListener(const eastl::string& filePath, std::weak_ptr<std::function<void()>> func);

private:

	FileModificationManager() {}
	~FileModificationManager();

	struct WriteTime
	{
		unsigned long dwLowDateTime;
		unsigned long dwHighDateTime;
	} m_lastWriteTime;

private:
	
	// Maps filepath to a map of owner pointers to the listeners.
	
	static eastl::hash_map<eastl::string, owner<FileModificationListener*>> s_listeners;
};