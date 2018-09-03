#pragma once

#include "Core.h"
#include "DatabaseAPI.h"

class IDatabase;

class DATABASE_API DatabaseModule
{
public:

	static owner<IDatabase*> createDatabase();
	static void destroyDatabase(owner<IDatabase*> database);

private:

	DatabaseModule() {}
	~DatabaseModule() {}
};