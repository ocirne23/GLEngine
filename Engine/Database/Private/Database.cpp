#include "Database.h"

owner<IDatabase*> Database::create()
{
	return owner<IDatabase*>();
}
