#include "Network/PacketSplitter.h"

#include "Network/TCPReceiveSocket.h"

void PacketSplitter::process()
{
	if (m_receiveSocket.isConnected())
	{
		if (m_header.isValid())
		{

		}
		else
		{
			m_receiveSocket.read(as_span(rcast<byte*>(&m_header), sizeof(m_header)));
		}
	}
}

void PacketSplitter::setPacketReceiver(PacketReceiver a_receiver)
{
}

void PacketSplitter::clear()
{

}