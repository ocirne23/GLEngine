#pragma once

#include "Core.h"

class Protocol
{
public:

	typedef ushort PacketID;

	typedef std::function<uint(PacketID)> PacketSizeParser;
	typedef std::function<void(span<const byte>)> PacketReceiver;

	enum { MAX_PACKET_SIZE = 512, BUFFER_SIZE = MAX_PACKET_SIZE * 2 };

	Protocol();
	~Protocol();

	void process(span<const byte> data);
	void setPacketSizeParser(PacketSizeParser sizeParser);
	void setPacketReceiver(PacketReceiver receiver);
	void tryFlush();

private:

	PacketSizeParser m_packetSizeParser;
	PacketReceiver m_packetReceiver;

	size_t m_usedSize = 0;
};