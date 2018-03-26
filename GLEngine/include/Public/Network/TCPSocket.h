#pragma once

#include "Core.h"
#include "Utils/RingQueue.h"

class TCPSocket
{
public:

	enum { BUFFER_SIZE = 512 };

	typedef not_null<const char*> Address;
	typedef int Port;

	TCPSocket() {}
	~TCPSocket();

	void listen(Address ip, Port port);
	void connect(Address ip, Port port);
	void disconnect();

	void send(span<const byte> data);
	bool receive();
	bool read(span<byte> data);

	size_t getMessageQueueSize() const { return m_ringQueue.size(); }

private:

	uint64 m_socket = 0;
	RingQueue<byte, BUFFER_SIZE> m_ringQueue;
};