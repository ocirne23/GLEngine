#pragma once

#include "Network/INetwork.h"

class Network final : public INetwork
{
	virtual owner<INetwork*> create() override;
};