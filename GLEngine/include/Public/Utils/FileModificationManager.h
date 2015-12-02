#pragma once

#include "EASTL/string.h"
#include "EASTL/hash_map.h"

#include <functional>

class FileModificationListener;

class FileModificationManager
{
public:
	friend class FileModificationListener;

	static void update();
	static void createModificationListener(void* ownerPtr, const eastl::string& filePath, std::function<void()> func);
	static void removeModificationListener(void* ownerPtr, const eastl::string& filePath);
	static void removeAllModificationListenersForOwner(void* ownerPtr);

private:

	FileModificationManager() {}
	~FileModificationManager() {}

	struct WriteTime
	{
		unsigned long dwLowDateTime;
		unsigned long dwHighDateTime;
	} m_lastWriteTime;

private:
	
	// Maps filepath to a map of owner pointers to the listeners.
	static eastl::hash_map<eastl::string, eastl::hash_map<void*, FileModificationListener*>*> s_listeners;
};