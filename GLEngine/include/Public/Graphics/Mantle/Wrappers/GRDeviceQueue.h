#pragma once

#include "Core.h"

class GRDeviceQueue
{
public:

	enum EQueueType { UNIVERSAL = 0, COMPUTE, NUM_QUEUE_TYPES, NONE };

	GRDeviceQueue() {}
	void initialize(EQueueType type);

	EQueueType getType() const { return m_type;  }
	uint64 getGRQueueID() const { return m_grQueueID; }

private:

	friend class GRDevice;
	void onDeviceCreation(uint64 grQueueID);

private:

	EQueueType m_type  = EQueueType::NONE;
	uint64 m_grQueueID = 0;
};