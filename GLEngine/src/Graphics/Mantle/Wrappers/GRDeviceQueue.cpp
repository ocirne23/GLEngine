#include "Graphics/Mantle/Wrappers/GRDeviceQueue.h"

void GRDeviceQueue::initialize(EQueueType a_type)
{
	m_type = a_type;
}

void GRDeviceQueue::onDeviceCreation(uint64 a_grQueueID)
{
	m_grQueueID = a_grQueueID;
}

