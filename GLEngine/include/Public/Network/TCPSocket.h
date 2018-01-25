#pragma once

#include "Core.h"

class TCPSocket
{
public:

	typedef gsl::not_null<const char*> Address;
	typedef int Port;

	TCPSocket() {}
	~TCPSocket();

	void listen(Address ip, Port port);
	void connect(Address ip, Port port);
	void disconnect();

	void send(gsl::span<const byte> data);
	bool receive(gsl::span<byte> receiveBuffer, std::function<void(gsl::span<byte>)> callback);

private:

	uint64 m_socket = 0;
};