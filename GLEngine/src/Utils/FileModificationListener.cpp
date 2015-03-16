#pragma once

#include "Utils/FileHandle.h"
#include "Utils/FileModificationListener.h"

#include "Core.h"

FileModificationListener::FileModificationListener(const rde::string& a_filePath, std::function<void()> a_onFileModification)
	: m_filePath(rde::string(FileHandle::ASSETS_DIR).append(a_filePath)), m_onFileModification(a_onFileModification)
{}