#include "Filesystem.h"

owner<IFilesystem*> IFilesystem::create()
{
	return new Filesystem();
}