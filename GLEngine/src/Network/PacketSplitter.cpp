#include "Network/PacketSplitter.h"

#include "Network/TCPReceiveSocket.h"

void PacketSplitter::process()
{
	if (m_receiveSocket.isConnected())
	{
		Header h;
		
	}
}

void PacketSplitter::setPacketReceiver(PacketReceiver a_receiver)
{
}

void PacketSplitter::clear()
{

}