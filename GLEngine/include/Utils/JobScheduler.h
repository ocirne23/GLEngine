#pragma once

#include <functional>
#include "rde/list.h"

enum class EJobRepeat 
{ 
	REPEAT, 
	NOREPEAT
};

class JobScheduler
{
public:

	void update(float deltaSec);
	void addJob(float timeIntervalSec, std::function<EJobRepeat(float, void*)> func, void* ptr = NULL);
private:

	struct Job
	{
		void* dataPtr;
		float timeIntervalSec;
		float timeAccumulator;
		std::function<EJobRepeat(float, void*)> func;
	};

	rde::list<Job> m_jobs;
};