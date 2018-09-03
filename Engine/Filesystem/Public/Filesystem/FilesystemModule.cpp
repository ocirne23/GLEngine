#include "FilesystemModule.h"

owner<IFilesystem*> FilesystemModule::createFilesystem()
{
	return owner<IFilesystem*>();
}

void FilesystemModule::destroyFilesystem(owner<IFilesystem*> filesystem)
{
}
