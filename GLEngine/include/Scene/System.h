#pragma once

#include "Core.h"

class System
{
public:

	System() : m_systemID(0xFFFF) {}
	~System() {}
	System(const System& copy) = delete;

	virtual void update(float deltaSec) = 0;

private:

	ushort m_systemID;
};