#include "Utils/FileModificationListener.h"

#include "Utils/FileHandle.h"

#define NOMINMAX
#include <Windows.h>
#include <fileapi.h>
#include <assert.h>

FileModificationListener::FileModificationListener(const eastl::string& a_filePath)
{
	WIN32_FILE_ATTRIBUTE_DATA data;
	m_fullFilePath = eastl::string(FileHandle::ASSETS_DIR).append(a_filePath);
	const BOOL result = GetFileAttributesEx(m_fullFilePath.c_str(), GET_FILEEX_INFO_LEVELS::GetFileExInfoStandard, &data);
	assert(result);

	m_lastWriteTime.dwLowDateTime = data.ftLastWriteTime.dwLowDateTime;
	m_lastWriteTime.dwHighDateTime = data.ftLastWriteTime.dwHighDateTime;
}

void FileModificationListener::update()
{
	WIN32_FILE_ATTRIBUTE_DATA data;
	const BOOL result = GetFileAttributesEx(m_fullFilePath.c_str(), GET_FILEEX_INFO_LEVELS::GetFileExInfoStandard, &data);
	assert(result);

	for (auto it = m_callbackList.begin(); it != m_callbackList.end(); )
	{
		std::weak_ptr<std::function<void()>> callback = *it;
		if (callback.expired())
		{
			it = m_callbackList.erase(it);
		}
		else if (m_lastWriteTime.dwLowDateTime != data.ftLastWriteTime.dwLowDateTime || m_lastWriteTime.dwHighDateTime != data.ftLastWriteTime.dwHighDateTime)
		{
			auto func = callback.lock();
			(*func.get())();
			++it;

			m_lastWriteTime.dwLowDateTime = data.ftLastWriteTime.dwLowDateTime;
			m_lastWriteTime.dwHighDateTime = data.ftLastWriteTime.dwHighDateTime;
		}
	}
}

void FileModificationListener::addCallback(std::weak_ptr<std::function<void()>> a_onFileModification)
{
	m_callbackList.push_back(a_onFileModification);
}
