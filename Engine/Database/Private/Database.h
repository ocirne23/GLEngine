#pragma once

#include "Database/IDatabase.h"

class Database final : public IDatabase
{
	virtual owner<IDatabase*> create() override;
};