#pragma once

#include "Core.h"
#include "FilesystemAPI.h"

class FilePath;

class FILESYSTEM_API IFilesystem
{
public:

	static owner<IFilesystem*> create();
};