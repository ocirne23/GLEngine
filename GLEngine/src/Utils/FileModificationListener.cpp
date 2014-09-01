#pragma once

#include "Utils\FileHandle.h"
#include "Utils\FileModificationListener.h"

FileModificationListener::FileModificationListener(const FileHandle& handle, std::function<void()> onFileModification)
{
	m_onFileModification = onFileModification;
	m_filePath = handle.getFilePath();
	print("listener: %s \n", m_filePath.c_str());
}