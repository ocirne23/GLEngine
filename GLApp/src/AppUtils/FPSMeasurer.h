#pragma once

#include "Core.h"
#include <functional>

class FPSMeasurer
{
public:

	FPSMeasurer(float timeInterval, std::function<void(const FPSMeasurer&)> timeIntervalReachedFunc)
		: m_timeInterval(timeInterval), m_timeIntervalReachedFunc(timeIntervalReachedFunc) {}

	void tickFrame(float deltaSec)
	{
		m_frameCounter++;
		m_timeAccumulator += deltaSec;
		if (m_timeAccumulator >= m_timeInterval)
		{
			m_timeIntervalReachedFunc(*this);
			m_timeAccumulator -= m_timeInterval;
			m_frameCounter = 0;
		}
	}

	uint getNumFramesPassed() const	{ return m_frameCounter; }
	float getTimePassed() const		{ return m_timeAccumulator; }
	float getTimeInterval() const	{ return m_timeInterval; }

private:

	std::function<void(const FPSMeasurer&)> m_timeIntervalReachedFunc;
	uint m_frameCounter		= 0;
	float m_timeAccumulator = 0;
	float m_timeInterval	= 0;
};