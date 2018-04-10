#pragma once

#include "Core.h"
#include "Utils/RingQueue.h"

class TCPSocket
{
public:


	typedef not_null<const char*> Address;
	typedef int Port;

	TCPSocket() {}
	virtual ~TCPSocket();

	void listen(Address ip, Port port);
	void connect(Address ip, Port port);
	void disconnect();
	bool isConnected() const { return m_socket != 0; }

	void send(span<const byte> data);
	int receive(span<byte> buffer);

protected:

	uint64 m_socket = 0;
};