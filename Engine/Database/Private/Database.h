#pragma once

#include "Core.h"
#include "Database/IDatabase.h"

class Database : public IDatabase
{
public:

private:

	friend class DatabaseModule;
	virtual ~Database() override {}
};