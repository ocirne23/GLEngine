#pragma once

#include "Core.h"
#include "FilesystemAPI.h"

class IFilesystem;

class FILESYSTEM_API FilesystemModule
{
public:

	static owner<IFilesystem*> createFilesystem();
	static void destroyFilesystem(owner<IFilesystem*> filesystem);

private:

	FilesystemModule() {}
	~FilesystemModule() {}
};