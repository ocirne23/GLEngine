#pragma once

#include "Core.h"
#include "Network/NetworkAPI.h"

class INetwork;

class NETWORK_API NetworkModule
{
public:

	static owner<INetwork*> createNetwork();
	static void destroyNetwork(owner<INetwork*> network);

private:

	NetworkModule() {}
	~NetworkModule() {}
};