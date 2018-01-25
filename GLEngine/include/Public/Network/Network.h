#pragma once

#include "Core.h"

class Network
{
	friend class GLEngine;
public:

	~Network();

	bool isInitialized() const { return m_isInitialized; }

private:

	Network();

private:

	bool m_isInitialized = false;
};