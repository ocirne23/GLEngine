#pragma once

#include "Network/TCPSocket.h"

template <size_t BufferSize = 512>
class TCPReceiveSocket : public TCPSocket
{
public:

	bool receive()
	{
		if (!m_socket)
			return false;
#if USE_COPY_BUFFER
		byte buf[BufferSize / 2];
		size_t maxReceivable = eastl::min(m_ringQueue.capacity() - m_ringQueue.size(), ARRAY_SIZE(buf));
#else
		byte* buf = m_ringQueue.getTail();
		size_t maxReceivable = m_ringQueue.getNumEmptyFromTailToEnd();
#endif
		if (maxReceivable > 0)
		{
			int numReceived = TCPSocket::receive(as_span(buf, maxReceivable));
			if (numReceived)
			{
#if USE_COPY_BUFFER
				bool r = m_ringQueue.push_back(buf, numReceived);
				assert(r);
#else
				m_ringQueue.appendToTail(numReceived);
#endif
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			print("Socket receive failed because queue is full!\n");
			return false;
		}
	}

	bool read(span<byte> data)
	{
		return m_ringQueue.pop_front(data.data(), data.length_bytes());
	}

	size_t getMessageQueueSize() const 
	{ 
		return m_ringQueue.size(); 
	}

	constexpr size_t getMessageQueueCapacity() const 
	{ 
		return m_ringQueue.capacity(); 
	}

private:

	RingQueue<byte, BufferSize> m_ringQueue;
};