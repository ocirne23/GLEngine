#pragma once

#include "Core.h"
#include "Utils/RingQueue.h"

template <size_t>
class TCPReceiveSocket;

class PacketSplitter
{
public:

	enum { RECEIVE_BUFFER_SIZE = 512 };
	typedef TCPReceiveSocket<RECEIVE_BUFFER_SIZE> ReceiveSocket;

	struct Header
	{
		short id = 0;

		bool isValid() const { return id != 0; }
		void invalidate() { id = 0; }
	};

	typedef std::function<void(span<byte>)> PacketReceiver;

	PacketSplitter(ReceiveSocket& a_receiveSocket)
		: m_receiveSocket(a_receiveSocket) 
	{}
	
	~PacketSplitter() {}

	void process();
	void setPacketReceiver(PacketReceiver receiver);
	void clear();

private:

	ReceiveSocket& m_receiveSocket;
	Header m_header;
};