#pragma once

#include "3rdparty/rde/rde_string.h"
#include "3rdparty/rde/hash_map.h"

#include <functional>

class FileModificationListener;

class FileModificationManager
{
public:
	friend class FileModificationListener;

	static void update();
	static void createModificationListener(void* ownerPtr, const rde::string& filePath, std::function<void()> func);
	static void removeModificationListener(void* ownerPtr, const rde::string& filePath);

private:

	FileModificationManager() {}
	~FileModificationManager() {}

	struct WriteTime
	{
		unsigned long dwLowDateTime;
		unsigned long dwHighDateTime;
	} m_lastWriteTime;

private:

	static rde::hash_map<rde::string, FileModificationListener*> s_listeners;
};