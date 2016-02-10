#pragma once

#include "Core.h"

class GRPhysicalGPU
{
public:

	GRPhysicalGPU() {}
	void initialize(uint64 grPhysicalGPUID);

	uint64 getPhysicalGPUID() const { return m_grPhysicalGPU; }

private:

	uint64 m_grPhysicalGPU = 0;
};