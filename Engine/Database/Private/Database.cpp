#include "Database.h"

owner<IDatabase*> IDatabase::create()
{
	return new Database();
}