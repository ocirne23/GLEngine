#pragma once

#include "Core.h"

#include <functional>

class FPSMeasurer
{
public:

	void setLogFunction(float a_timeInterval, std::function<void(const FPSMeasurer&)> a_logFunc)
	{
		m_timeInterval = a_timeInterval;
		m_logFunc = a_logFunc;
	}

	void tickFrame(float deltaSec)
	{
		m_frameCounter++;
		m_timeAccumulator += deltaSec;
		if (m_timeAccumulator >= m_timeInterval)
		{
			m_logFunc(*this);
			m_timeAccumulator -= m_timeInterval;
			m_frameCounter = 0;
		}
	}

	uint getNumFramesPassed() const { return m_frameCounter; }
	float getTimePassed() const     { return m_timeAccumulator; }
	float getTimeInterval() const   { return m_timeInterval; }
	uint getAvgFps() const          { return (uint) (m_frameCounter / m_timeInterval); }
	float getAvgMsPerFrame() const  { return m_timeAccumulator / m_frameCounter; }

private:

	std::function<void(const FPSMeasurer&)> m_logFunc;
	uint m_frameCounter     = 0;
	float m_timeAccumulator = 0;
	float m_timeInterval    = 0;
};