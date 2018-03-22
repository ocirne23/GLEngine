#pragma once

#include "Core.h"
#include "Utils/RingQueue.h"

class TCPSocket
{
public:

	enum { BUFFER_SIZE = 64 };

	typedef gsl::not_null<const char*> Address;
	typedef int Port;

	TCPSocket() {}
	~TCPSocket();

	void listen(Address ip, Port port);
	void connect(Address ip, Port port);
	void disconnect();

	void send(gsl::span<const byte> data);
	bool receive();

	size_t getMessageQueueSize() const { return m_ringQueue.size(); }
	bool readFromMessageQueue(span<byte> data)
	{
		return m_ringQueue.pop_front(data.data(), data.length_bytes());
	}

	uint64 m_socket = 0;
	RingQueue<byte, BUFFER_SIZE> m_ringQueue;

private:


};