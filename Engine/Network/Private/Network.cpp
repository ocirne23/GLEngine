#include "Network.h"

owner<INetwork*> INetwork::create()
{
	return new Network();
}