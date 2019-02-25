#include "Threading.h"

owner<IThreading*> IThreading::create()
{
	return new Threading();
}