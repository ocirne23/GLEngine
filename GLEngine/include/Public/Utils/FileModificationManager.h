#pragma once

#include "gsl/gsl.h"
#include "EASTL/string.h"
#include "EASTL/hash_map.h"

#include <functional>

class FileModificationListener;

class FileModificationManager
{
public:

	friend class GLEngine;

	/* The lambda will be notified of changes as long as the shared_ptr is alive */
	static std::shared_ptr<std::function<void()>> createModificationCallback(const eastl::string& filePath, std::function<void()> func);

private:

	FileModificationManager() {}
	~FileModificationManager();

	static void update();

private:
	
	// Maps filepath to a map of owner pointers to the listeners.
	static eastl::hash_map<eastl::string, owner<FileModificationListener*>> s_listeners;
};