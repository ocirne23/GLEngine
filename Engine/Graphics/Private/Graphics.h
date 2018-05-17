#pragma once

#include "IGraphics.h"

class Graphics final : public IGraphics
{
	virtual owner<IGraphics*> create() override;
};