#include "Network/NetworkModule.h"

#include "Network.h"

owner<INetwork*> NetworkModule::createNetwork()
{
	return owner<INetwork*>(new Network());
}

void NetworkModule::destroyNetwork(owner<INetwork*> network)
{
	assert(network);
	delete static_cast<Network*>(network);
}
