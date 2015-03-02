#include "Utils/FileModificationManager.h"

#include "Core.h"
#include "rde/rde_string.h"
#include "Utils/FileModificationListener.h"

#include <Windows.h>

rde::hash_map<rde::string, FileModificationListener*> FileModificationManager::s_listeners;

void FileModificationManager::update()
{
	for (auto& listener : s_listeners)
	{
		WIN32_FILE_ATTRIBUTE_DATA data;
		BOOL result = GetFileAttributesEx(listener.second->m_filePath.c_str(), GET_FILEEX_INFO_LEVELS::GetFileExInfoStandard, &data);
		assert(result);

		if (listener.second->m_lastWriteTime.dwLowDateTime != data.ftLastWriteTime.dwLowDateTime ||
			listener.second->m_lastWriteTime.dwHighDateTime != data.ftLastWriteTime.dwHighDateTime)
		{
			listener.second->m_onFileModification();
			listener.second->m_lastWriteTime.dwLowDateTime = data.ftLastWriteTime.dwLowDateTime;
			listener.second->m_lastWriteTime.dwHighDateTime = data.ftLastWriteTime.dwHighDateTime;
		}
	}
}

void FileModificationManager::createModificationListener(void* a_ownerPtr, const rde::string& a_filePath, std::function<void()> a_func)
{
	FileModificationListener* listener = new FileModificationListener(a_filePath, a_func);
	rde::string key = rde::to_string((uint64)a_ownerPtr).append(rde::string(a_filePath));
	s_listeners.insert({ key , listener });

	WIN32_FILE_ATTRIBUTE_DATA data;
	BOOL result = GetFileAttributesEx(listener->m_filePath.c_str(), GET_FILEEX_INFO_LEVELS::GetFileExInfoStandard, &data);
	assert(result);

	listener->m_lastWriteTime.dwLowDateTime = data.ftLastWriteTime.dwLowDateTime;
	listener->m_lastWriteTime.dwHighDateTime = data.ftLastWriteTime.dwHighDateTime;
}

void FileModificationManager::removeModificationListener(void* a_ownerPtr, const rde::string& a_filePath)
{
	rde::string key = rde::to_string((uint64)a_ownerPtr).append(rde::string(a_filePath));

	auto it = s_listeners.find(key);
	assert(it != s_listeners.end());
	s_listeners.erase(it);
	delete it->second;
}