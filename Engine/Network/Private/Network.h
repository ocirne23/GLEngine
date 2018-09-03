#pragma once

#include "Core.h"
#include "Network/INetwork.h"

class Network : public INetwork
{
public:

private:
	
	friend class NetworkModule;
	Network() {}
	virtual ~Network() override {}
};