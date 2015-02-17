#pragma once

#include "rde/rde_string.h"
#include "rde/vector.h"

#include <functional>

class FileModificationListener;

class FileModificationManager
{
public:
	friend class FileModificationListener;
	
	static void update();
	static FileModificationListener* createModificationListener(const rde::string& filePath, std::function<void()> func);
	static void removeModificationListener(FileModificationListener* listener);

private:

	FileModificationManager() {}
	~FileModificationManager() {}

	struct WriteTime
	{
		unsigned long dwLowDateTime;
		unsigned long dwHighDateTime;
	} m_lastWriteTime;

private:

	static rde::vector<FileModificationListener*> s_listeners;
};