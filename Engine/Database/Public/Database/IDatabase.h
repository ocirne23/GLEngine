#pragma once

class IDatabase
{
public:

protected:

	friend class DatabaseModule;
	virtual ~IDatabase() {}
};