#pragma once

#include "Core.h"
#include "EASTL/deque.h"

#include <assert.h>
#include <chrono>
#include <type_traits>

class Stopwatch
{
public:

	Stopwatch(uint a_averageOverNumIterations = 1) : m_averageOverNumIterations(a_averageOverNumIterations) {}
	~Stopwatch() {}

	void start()
	{
		assert(!m_isStarted);
		m_startTime = m_clock.now();
		m_isStarted = true;
	}

	void stop()
	{
		assert(m_isStarted);
		m_stopTime = m_clock.now();
		m_isStarted = false;

		std::chrono::microseconds elapsed = std::chrono::duration_cast<std::chrono::microseconds>(m_stopTime - m_startTime);
		m_elapsedTimes.push_back(elapsed);
		m_totalElapsed += elapsed;
		m_currentNumIterations++;

		if (m_currentNumIterations > m_averageOverNumIterations)
		{
			m_totalElapsed -= m_elapsedTimes.front();
			m_elapsedTimes.pop_front();
			m_currentNumIterations--;
		}
	}

	void reset()
	{
		assert(!m_isStarted);
		m_currentNumIterations = 0;
		m_elapsedTimes.clear();
		m_totalElapsed = std::chrono::microseconds();
	}

	std::chrono::microseconds avgMicroSec()
	{
		return m_totalElapsed / m_currentNumIterations;
	}

private:

	bool m_isStarted = false;
	uint m_averageOverNumIterations = 0;
	uint m_currentNumIterations = 0;

	eastl::deque<std::chrono::microseconds> m_elapsedTimes;
	std::chrono::microseconds m_totalElapsed = std::chrono::microseconds();

	std::chrono::high_resolution_clock m_clock;
	std::chrono::high_resolution_clock::time_point m_startTime;
	std::chrono::high_resolution_clock::time_point m_stopTime;

};