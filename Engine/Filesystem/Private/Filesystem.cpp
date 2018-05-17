#include "Filesystem.h"

owner<IFilesystem*> Filesystem::create()
{
	return owner<IFilesystem*>();
}
