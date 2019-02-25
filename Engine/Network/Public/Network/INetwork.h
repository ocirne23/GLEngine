#pragma once

#include "Core.h"
#include "NetworkAPI.h"

class NETWORK_API INetwork
{
public:
	
	static owner<INetwork*> create();
};