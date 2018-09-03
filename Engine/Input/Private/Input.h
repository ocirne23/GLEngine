#pragma once

#include "Core.h"
#include "Input/IInput.h"

class Input : public IInput
{
public:

private:

	friend class InputModule;
	virtual ~Input() override {}
};