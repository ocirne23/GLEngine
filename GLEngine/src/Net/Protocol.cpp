#include "Network/Protocol.h"

Protocol::Protocol()
{
}

void Protocol::process(span<const byte> a_data)
{
	bool fits = m_ringQueue.push_back(a_data.data(), a_data.size_bytes());
	assert(fits);
	tryFlush();
}

void Protocol::tryFlush()
{
	if (!m_ringQueue.size())
		return;
	uint nextPacketSize = uint(*m_ringQueue.peek_front());
	assert(nextPacketSize > 0);
	if (nextPacketSize <= m_ringQueue.size())
	{
		bool ret = m_ringQueue.pop_front(m_outputBuffer, nextPacketSize);
		assert(ret);
		m_packetReceiver(gsl::span<byte>(m_outputBuffer, nextPacketSize));
	}
}

void Protocol::setPacketSizeParser(PacketSizeParser a_parser)
{
	m_packetSizeParser = a_parser;
}

void Protocol::setPacketReceiver(PacketReceiver a_receiver)
{
	m_packetReceiver = a_receiver;
}