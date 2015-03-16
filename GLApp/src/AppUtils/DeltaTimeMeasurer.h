#pragma once

#include "GLEngine.h"

class DeltaTimeMeasurer
{
public:

    float calcDeltaSec(uint currTimeMs)
    {
	if (m_startTime == 0xFFFFFFFF)
	    m_startTime = currTimeMs;

	uint currentTime = currTimeMs;
	m_deltaSec = (currentTime - m_startTime) / 1000.0f;
	m_startTime = currentTime;

	return m_deltaSec;
    }

    float getDeltaSec() const { return m_deltaSec; }

private:

    float m_deltaSec;
    uint m_startTime = 0xFFFFFFFF;
};