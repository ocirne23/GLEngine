#pragma once

#include "IInput.h"

class Input final : public IInput
{
	virtual owner<IInput*> create() override;
};