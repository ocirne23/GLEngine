#pragma once

#include "Threading/IThreading.h"

class Threading : public IThreading
{
public:

private:

	friend class ThreadingModule;
	Threading() {}
	virtual ~Threading() override {}
};