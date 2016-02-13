#include "Utils/FileModificationListener.h"

#include "Utils/FileHandle.h"

#define NOMINMAX
#include <Windows.h>
#include <fileapi.h>
#include <assert.h>

BEGIN_UNNAMED_NAMESPACE()

FileModificationListener::WriteTime getWriteTime(const eastl::string& a_filePath)
{
	WIN32_FILE_ATTRIBUTE_DATA data;
	const BOOL result = GetFileAttributesEx(a_filePath.c_str(), GET_FILEEX_INFO_LEVELS::GetFileExInfoStandard, &data);
	assert(result);

	FileModificationListener::WriteTime writeTime;
	writeTime.dwLowDateTime = data.ftLastWriteTime.dwLowDateTime;
	writeTime.dwHighDateTime = data.ftLastWriteTime.dwHighDateTime;
	return writeTime;
}

END_UNNAMED_NAMESPACE()

FileModificationListener::FileModificationListener(const eastl::string& a_filePath)
{
	m_fullFilePath = eastl::string(FileHandle::ASSETS_DIR).append(a_filePath);
	m_lastWriteTime = getWriteTime(m_fullFilePath);
}

void FileModificationListener::update()
{
	WriteTime currWriteTime = getWriteTime(m_fullFilePath);
	bool changed = (m_lastWriteTime.dwLowDateTime != currWriteTime.dwLowDateTime || m_lastWriteTime.dwHighDateTime != currWriteTime.dwHighDateTime);
	if (changed)
		m_lastWriteTime = currWriteTime;
	for (auto it = m_callbackList.begin(); it != m_callbackList.end(); )
	{
		std::weak_ptr<std::function<void()>> callback = *it;
		if (callback.expired())
		{
			it = m_callbackList.erase(it);
		}
		else
		{
			++it;
			if (changed)
			{
				auto func = callback.lock();
				func->operator()();
			}
		}
	}
}

void FileModificationListener::addCallback(std::weak_ptr<std::function<void()>> a_onFileModification)
{
	m_callbackList.push_back(a_onFileModification);
}
