#pragma once

#include <rde\vector.h>

#include <functional>

class FileHandle;
class FileModificationListener;

class FileModificationManager
{
public:
	friend class FileModificationListener;
	
	static void update();

	static FileModificationListener* createModificationListener(const FileHandle& handle, std::function<void()> func);
	static void removeModificationListener(FileModificationListener* listener);

private:


	FileModificationManager() {}
	~FileModificationManager() {}

private:

	static rde::vector<FileModificationListener*> s_listeners;
};