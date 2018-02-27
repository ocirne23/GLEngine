#include "Network/Protocol.h"

void Protocol::process(span<const byte> a_data)
{
	bool fits = m_ringQueue.push_back(a_data.data(), a_data.size_bytes());
	assert(fits);
	tryFlush();
}

void Protocol::tryFlush()
{
	if (!m_header.size && m_ringQueue.size() >= sizeof(m_header))
	{
		bool result = m_ringQueue.pop_front(rcast<byte*>(&m_header), sizeof(m_header));
		assert(result);
	}		

	if (m_header.size && m_ringQueue.size() >= m_header.size)
	{
		bool result = m_ringQueue.pop_front(m_outputBuffer, m_header.size);
		assert(result);
		m_packetReceiver(gsl::span<byte>(m_outputBuffer, m_header.size));
		m_header.size = 0;
	}
}

void Protocol::setPacketReceiver(PacketReceiver a_receiver)
{
	m_packetReceiver = a_receiver;
}

void Protocol::clear()
{
	m_header.size = 0;
	m_ringQueue.clear();
}