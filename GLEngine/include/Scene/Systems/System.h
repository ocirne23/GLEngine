#pragma once

#include "Core.h"

class System
{
public:

	System(uint systemID) : m_systemID(systemID) {}
	~System() {}
	System(const System& copy) = delete;

	virtual void update(float deltaSec) = 0;

private:

	const ushort m_systemID;
};