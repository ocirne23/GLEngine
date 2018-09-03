#include "Database/DatabaseModule.h"

#include "Database.h"

owner<IDatabase*> DatabaseModule::createDatabase()
{
	return owner<IDatabase*>(new Database());
}

void DatabaseModule::destroyDatabase(owner<IDatabase*> database)
{
	assert(database);
	delete static_cast<Database*>(database);
}
