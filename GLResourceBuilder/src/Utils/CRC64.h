#pragma once

#include "BuilderCore.h"

class CRC64
{
public:
	static uint64 getHash(const char* str);
private:
	CRC64() {}
	~CRC64() {}
};

