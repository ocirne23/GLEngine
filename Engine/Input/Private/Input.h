#pragma once

#include "Input/IInput.h"

class Input final : public IInput
{
	virtual owner<IInput*> create() override;
};