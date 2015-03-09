#include "Utils/JobScheduler.h"

void JobScheduler::update(float a_deltaSec)
{
	for (auto it = m_jobs.begin(); it != m_jobs.end();)
	{
		Job& job = *it;
		job.timeAccumulator += a_deltaSec;
		if (job.timeAccumulator > job.timeIntervalSec)
		{
			job.timeAccumulator = 0.0f;
			
			if (job.func(job.dataPtr) == EJobRepeat_NOREPEAT)
				it = m_jobs.erase(it);
			else
				++it;
		}
		else
		{
			++it;
		}
	}
}

void JobScheduler::addJob(float a_timeIntervalSec, std::function<EJobRepeat(void*)> a_func, void* a_ptr)
{
	Job job;
	job.dataPtr = a_ptr;
	job.func = a_func;
	job.timeIntervalSec = a_timeIntervalSec;
	job.timeAccumulator = 0.0f;
	m_jobs.push_back(job);
}