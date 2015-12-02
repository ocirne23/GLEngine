#include "Utils/FileModificationManager.h"

#include "Core.h"
#include "Utils/FileModificationListener.h"
#include "Utils/FileHandle.h"

#include <assert.h>
#include <Windows.h>

eastl::hash_map<eastl::string, eastl::hash_map<void*, FileModificationListener*>*>FileModificationManager::s_listeners;

void FileModificationManager::update()
{
	for (auto& files : s_listeners)
	{
		eastl::string fullFilePath = eastl::string(FileHandle::ASSETS_DIR).append(files.first);
		WIN32_FILE_ATTRIBUTE_DATA data;
		const BOOL result = GetFileAttributesEx(fullFilePath.c_str(), GET_FILEEX_INFO_LEVELS::GetFileExInfoStandard, &data);
		assert(result);
		
		// This can be optimized
		eastl::hash_map<void*, FileModificationListener*>* ownerListenerMap = files.second;
		assert(ownerListenerMap);
		for (auto& listeners : *ownerListenerMap) 
		{
			FileModificationListener* listener = listeners.second;
			if (listener->m_lastWriteTime.dwLowDateTime != data.ftLastWriteTime.dwLowDateTime ||
				listener->m_lastWriteTime.dwHighDateTime != data.ftLastWriteTime.dwHighDateTime)
			{
				listener->m_onFileModification();
				listener->m_lastWriteTime.dwLowDateTime = data.ftLastWriteTime.dwLowDateTime;
				listener->m_lastWriteTime.dwHighDateTime = data.ftLastWriteTime.dwHighDateTime;
			}
		}
	}
}

void FileModificationManager::createModificationListener(void* a_ownerPtr, const eastl::string& a_filePath, std::function<void()> a_func)
{
	FileModificationListener* listener = new FileModificationListener(a_filePath, a_func);
	eastl::hash_map<void*, FileModificationListener*>* ownerListenerMap = NULL;

	// find ownerPtr to listener map based on filePath.
	auto it = s_listeners.find(a_filePath);
	if (it == s_listeners.end())
	{
		ownerListenerMap = new eastl::hash_map<void*, FileModificationListener*>();
		s_listeners.insert({a_filePath, ownerListenerMap});
	}
	else
		ownerListenerMap = (*it).second;

	ownerListenerMap->insert({a_ownerPtr, listener});

	// set the current file time in the listener.
	WIN32_FILE_ATTRIBUTE_DATA data;
	eastl::string fullFilePath = eastl::string(FileHandle::ASSETS_DIR).append(listener->m_filePath);
	const BOOL result = GetFileAttributesEx(fullFilePath.c_str(), GET_FILEEX_INFO_LEVELS::GetFileExInfoStandard, &data);
	assert(result);

	listener->m_lastWriteTime.dwLowDateTime = data.ftLastWriteTime.dwLowDateTime;
	listener->m_lastWriteTime.dwHighDateTime = data.ftLastWriteTime.dwHighDateTime;
}

void FileModificationManager::removeModificationListener(void* a_ownerPtr, const eastl::string& a_filePath)
{
	auto ownerMapIt = s_listeners.find(a_filePath);
	assert(ownerMapIt != s_listeners.end());

	eastl::hash_map<void*, FileModificationListener*>* ownerListenerMap = (*ownerMapIt).second;
	auto listenerIt = ownerListenerMap->find(a_ownerPtr);
	assert(listenerIt != ownerListenerMap->end());

	delete listenerIt->second;
	ownerListenerMap->erase(listenerIt);
	if (ownerListenerMap->empty())
		delete ownerListenerMap;
}

void FileModificationManager::removeAllModificationListenersForOwner(void* a_ownerPtr)
{
	auto filesIt = s_listeners.begin();
	while (filesIt != s_listeners.end())
	{
		eastl::hash_map<void*, FileModificationListener*>* ownerListenerMap = filesIt->second;
		assert(ownerListenerMap);
		auto listenerIt = ownerListenerMap->find(a_ownerPtr);
		if (listenerIt != ownerListenerMap->end())
		{
			delete listenerIt->second;
			ownerListenerMap->erase(listenerIt);
		}

		if (ownerListenerMap->empty())
		{
			delete ownerListenerMap;
			s_listeners.erase(filesIt++);
		}
		else
			++filesIt;
	}
}
