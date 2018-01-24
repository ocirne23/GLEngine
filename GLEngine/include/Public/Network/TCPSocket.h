#pragma once

#include "Core.h"

class TCPSocket
{
public:

	typedef const char* Address;
	typedef int Port;

	TCPSocket() {}
	~TCPSocket() {}

	void initialize(Address ip, Port port);

private:

	uint* m_socket = nullptr;
};