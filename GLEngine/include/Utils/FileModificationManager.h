#pragma once

#include "rde/rde_string.h"
#include "rde/hash_map.h"

#include <functional>

class FileModificationListener;

class FileModificationManager
{
public:
	friend class FileModificationListener;
	
	static void update();
	static void createModificationListener(void* ownerPtr, const rde::string& filePath, std::function<void()> func);
	static void removeModificationListener(void* ownerPtr);

private:

	FileModificationManager() {}
	~FileModificationManager() {}

	struct WriteTime
	{
		unsigned long dwLowDateTime;
		unsigned long dwHighDateTime;
	} m_lastWriteTime;

private:

	static rde::hash_map<void*, FileModificationListener*> s_listeners;
};