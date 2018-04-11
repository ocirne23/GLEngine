#pragma once

#include "Core.h"
#include "Utils/RingQueue.h"
#include "Network/TCPReceiveSocket.h"

#include <utility>

template <size_t>
class TCPReceiveSocket;

template <typename Header>
class IPacketReceiver
{
public:
	virtual ~IPacketReceiver() {}

	virtual size_t getPacketSize(const Header& header) = 0;
	virtual void receivePacket(span<byte>& data) = 0;
};

template <typename Header, size_t BufferSize, typename PacketReceiver, typename std::enable_if<std::is_base_of<IPacketReceiver<Header>, PacketReceiver>::value>::type* = nullptr>
class Protocol
{
public:

	typedef TCPReceiveSocket<BufferSize> ReceiveSocket;

	Protocol(ReceiveSocket& a_receiveSocket, PacketReceiver a_packetReceiver)
		: m_receiveSocket(a_receiveSocket)
		, m_packetReceiver(a_packetReceiver)
	{}
	
	~Protocol() {}

	void process()
	{
		if (m_receiveSocket.isConnected())
		{
			if (!m_nextPacketSize)
			{
				if (m_receiveSocket.read(as_span(rcast<byte*>(&m_header), sizeof(m_header))))
				{
					m_nextPacketSize = m_packetReceiver.getPacketSize(m_header);
					assert(m_nextPacketSize != 0);
				}
			}
			else if (m_receiveSocket.getMessageQueueSize() >= m_nextPacketSize)
			{
				span<byte> data;
				m_packetReceiver.receivePacket(data);
				assert(data.size_bytes() == m_nextPacketSize);
				bool readPacket = m_receiveSocket.read(data);
				assert(readPacket);
				m_nextPacketSize = 0;
			}
		}
	}

private:

	PacketReceiver& m_packetReceiver;
	ReceiveSocket& m_receiveSocket;
	Header m_header;
	size_t m_nextPacketSize = 0;
};