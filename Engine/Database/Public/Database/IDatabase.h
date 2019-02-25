#pragma once

#include "Core.h"
#include "DatabaseAPI.h"

class DATABASE_API IDatabase
{
public:

	static owner<IDatabase*> create();
};