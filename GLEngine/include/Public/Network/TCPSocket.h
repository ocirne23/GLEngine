#pragma once

#include "Core.h"

class TCPSocket
{
public:

	enum { BUFFER_SIZE = 512 };

	typedef const char* Address;
	typedef int Port;

	TCPSocket() {}
	~TCPSocket() {}

	void listen(Address ip, Port port, uint maxNumConnections);
	void connect(Address ip, Port port);

private:

	uint64 m_socket = 0;
	uint64 m_connectedSocket = 0;

	char m_receiveBuffer[BUFFER_SIZE];
};