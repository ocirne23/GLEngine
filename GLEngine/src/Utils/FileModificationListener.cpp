#pragma once

#include "Utils/FileModificationListener.h"

#include "Utils/FileHandle.h"

FileModificationListener::FileModificationListener(const FileHandle& a_handle, std::function<void()> a_onFileModification) 
	: m_filePath(a_handle.getFilePath()), m_onFileModification(a_onFileModification)
{
}