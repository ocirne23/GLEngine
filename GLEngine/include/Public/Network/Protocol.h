#pragma once

#include "Core.h"
#include "Utils/RingQueue.h"

class Protocol
{
public:

	struct Header
	{
		short size = 0;
	};

	typedef std::function<void(span<byte>)> PacketReceiver;

	enum { MAX_PACKET_SIZE = 512, BUFFER_SIZE = MAX_PACKET_SIZE * 2 };

	Protocol() {}
	~Protocol() {}

	void process(span<const byte> data);
	void setPacketReceiver(PacketReceiver receiver);
	void tryFlush();
	void clear();

private:

	Header m_header;
	PacketReceiver m_packetReceiver;
	byte m_outputBuffer[MAX_PACKET_SIZE];
	RingQueue<byte, BUFFER_SIZE> m_ringQueue;
};