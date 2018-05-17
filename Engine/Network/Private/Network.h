#pragma once

#include "INetwork.h"

class Network final : public INetwork
{
	virtual owner<INetwork*> create() override;
};