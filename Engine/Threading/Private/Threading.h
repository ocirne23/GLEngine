#pragma once

#include "IThreading.h"

class Threading final : public IThreading
{
	virtual owner<IThreading*> create() override;
};