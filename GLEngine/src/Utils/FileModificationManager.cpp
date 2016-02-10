#include "Utils/FileModificationManager.h"

#include "Utils/FileModificationListener.h"

eastl::hash_map<eastl::string, owner<FileModificationListener*>> FileModificationManager::s_listeners;

FileModificationManager::~FileModificationManager()
{
	for (auto it : s_listeners)
		delete it.second;
}

void FileModificationManager::update()
{
	for (auto it = s_listeners.begin(); it != s_listeners.end(); )
	{
		owner<FileModificationListener*> listener = it->second;
		listener->update();
		if (!listener->numCallbacks())
		{
			it = s_listeners.erase(it);
			delete listener;
		}
		else
			++it;
	}
}

void FileModificationManager::createModificationListener(const eastl::string& a_filePath, std::weak_ptr<std::function<void()>> a_func)
{
	auto it = s_listeners.find(a_filePath);
	if (it != s_listeners.end())
	{
		owner<FileModificationListener*> listener = it->second;
		listener->addCallback(a_func);
	}
	else
	{
		owner<FileModificationListener*> listener = new FileModificationListener(a_filePath);
		s_listeners.insert({ a_filePath, listener });
		listener->addCallback(a_func);
	}
}