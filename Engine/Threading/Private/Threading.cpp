#include "Threading.h"

owner<IThreading*> Threading::create()
{
	return owner<IThreading*>();
}
