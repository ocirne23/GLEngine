#pragma once

#include "GLEngine.h"

class DeltaTimeMeasurer
{
public:

	float calcDeltaSec(uint currTimeMs)
	{
		if (m_startTime == UNINITIALIZED_TIME)
			m_startTime = currTimeMs;

		uint currentTime = currTimeMs;
		m_deltaSec = (currentTime - m_startTime) / 1000.0f;
		m_startTime = currentTime;

		return m_deltaSec;
	}

	float getDeltaSec() const { return m_deltaSec; }

private:

	const int UNINITIALIZED_TIME = 0xFFFFFFFF;

	float m_deltaSec = 0.0f;
	uint m_startTime = UNINITIALIZED_TIME;
};