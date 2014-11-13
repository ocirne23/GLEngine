#pragma once

#include "Utils/FileHandle.h"
#include "Utils/FileModificationListener.h"

FileModificationListener::FileModificationListener(const FileHandle& a_handle, std::function<void()> a_onFileModification)
{
	m_onFileModification = a_onFileModification;
	m_filePath = a_handle.getFilePath();
}