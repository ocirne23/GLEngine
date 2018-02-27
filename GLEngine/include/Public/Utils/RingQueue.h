#pragma once

#include "Core.h"

#include <algorithm>

template <typename T, size_t Size>
class RingQueue
{
public:

	RingQueue() {}
	~RingQueue() {}

	bool push_back(const T* a_data, size_t a_numElements)
	{
		if (a_numElements > Size - m_count)
			return false;

		int64 numElementsAdded = 0;
		size_t tailIdx = (m_headIdx + m_count) % Size;
		int64 numFreeAtEnd = std::min(int64(Size) - tailIdx, a_numElements);
		if (numFreeAtEnd > 0)
		{
			std::memcpy(m_data + tailIdx, a_data, numFreeAtEnd * sizeof(T));
		}
		if (numFreeAtEnd < int64(a_numElements))
		{
			tailIdx = (tailIdx + numFreeAtEnd) % Size;
			std::memcpy(m_data + tailIdx, a_data + numFreeAtEnd, (a_numElements - numFreeAtEnd) * sizeof(T));
		}
		m_count += a_numElements;

		return true;
	}

	bool push_back(const T* a_begin, const T* a_end)
	{
		size_t numElements = end - begin;
		return push_back(a_data, numElements);
	}

	T* peek_front()
	{
		assert(m_count);
		return &m_data[m_headIdx];
	}

	T* pop_front()
	{
		T* ret = nullptr;
		if (m_count)
		{
			m_count--;
			ret = m_data[m_headIdx];
			if (m_count == 0)
				m_headIdx = 0;
		}
		return ret;
	}

	bool pop_front(T* a_outData, size_t a_numElements)
	{
		if (a_numElements > m_count)
			return false;

		int64 numUntillEnd = std::min(int64(Size) - m_headIdx, a_numElements);
		if (numUntillEnd > 0)
		{
			std::memcpy(a_outData, m_data + m_headIdx, numUntillEnd * sizeof(T));
		}
		if (numUntillEnd < int64(a_numElements))
		{
			m_headIdx = (m_headIdx + numUntillEnd) % Size;
			std::memcpy(a_outData + numUntillEnd, m_data + m_headIdx, (a_numElements - numUntillEnd) * sizeof(T));
		}
		m_count -= a_numElements;

		if (m_count == 0)
			m_headIdx = 0;

		return true;
	}

	T* getTail()
	{
		size_t tailIdx = (m_headIdx + m_count) % Size;
		return m_data + tailIdx;
	}

	void appendToTail(size_t a_num)
	{
		if (m_headIdx + m_count + a_num <= Size)
		{
			m_count += a_num;
		}
		else
		{
			assert(false);
		}
	}

	size_t getNumEmptyFromTailToEnd() const 
	{
		size_t tailIdx = (m_headIdx + m_count) % Size;
		if (tailIdx <= m_headIdx)
		{
			return Size - m_count;
		}
		else
		{
			return Size - tailIdx;
		}
	}

	void clear()
	{
		m_headIdx = 0;
		m_count = 0;
	}

	size_t size() const { return m_count; }
	size_t capacity() const { return Size; }

private:

	T m_data[Size];
	size_t m_headIdx = 0;
	size_t m_count = 0;
};