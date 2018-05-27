#pragma once

#include "Threading/IThreading.h"

class Threading final : public IThreading
{
	virtual owner<IThreading*> create() override;
};