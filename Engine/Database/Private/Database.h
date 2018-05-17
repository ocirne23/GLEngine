#pragma once

#include "IDatabase.h"

class Database final : public IDatabase
{
	virtual owner<IDatabase*> create() override;
};