#pragma once

#include "3rdparty/rde/rde_string.h"
#include "3rdparty/rde/hash_map.h"
#include "3rdparty/rde/vector.h"

#include <functional>

class FileModificationListener;

class FileModificationManager
{
public:
	friend class FileModificationListener;

	static void update();
	static void createModificationListener(void* ownerPtr, const rde::string& filePath, std::function<void()> func);
	static void removeModificationListener(void* ownerPtr, const rde::string& filePath);
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
	static rde::hash_map<rde::string, rde::hash_map<void*, FileModificationListener*>*> s_listeners;
};