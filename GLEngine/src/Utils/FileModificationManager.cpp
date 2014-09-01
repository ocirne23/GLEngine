#include "Utils\FileModificationManager.h"

#include "Utils\FileModificationListener.h"

#include "Core.h"

#include <Windows.h>

rde::vector<FileModificationListener*> FileModificationManager::s_listeners;

void FileModificationManager::update()
{
	for (FileModificationListener* listener : s_listeners)
	{
		WIN32_FILE_ATTRIBUTE_DATA data;
		BOOL result = GetFileAttributesEx(listener->m_filePath.c_str(), GET_FILEEX_INFO_LEVELS::GetFileExInfoStandard, &data);
		assert(result);

		if (listener->m_lastWriteTime.dwLowDateTime != data.ftLastWriteTime.dwLowDateTime ||
			listener->m_lastWriteTime.dwHighDateTime != data.ftLastWriteTime.dwHighDateTime)
		{
			listener->m_onFileModification();
			listener->m_lastWriteTime.dwLowDateTime = data.ftLastWriteTime.dwLowDateTime;
			listener->m_lastWriteTime.dwHighDateTime = data.ftLastWriteTime.dwHighDateTime;
		}
	}
}

FileModificationListener* FileModificationManager::createModificationListener(const FileHandle& handle, std::function<void()> func)
{
	FileModificationListener* listener = new FileModificationListener(handle, func);
	s_listeners.push_back(listener);

	WIN32_FILE_ATTRIBUTE_DATA data;
	BOOL result = GetFileAttributesEx(listener->m_filePath.c_str(), GET_FILEEX_INFO_LEVELS::GetFileExInfoStandard, &data);

	assert(result);
	listener->m_lastWriteTime.dwLowDateTime = data.ftLastWriteTime.dwLowDateTime;
	listener->m_lastWriteTime.dwHighDateTime = data.ftLastWriteTime.dwHighDateTime;

	return listener;
}

void FileModificationManager::removeModificationListener(FileModificationListener* listener)
{
	auto it = s_listeners.find(listener);
	assert(it != s_listeners.end());
	s_listeners.erase(it);
	delete listener;
}