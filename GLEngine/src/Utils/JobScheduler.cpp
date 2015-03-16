#include "Utils/JobScheduler.h"

void JobScheduler::update(float a_deltaSec)
{
	for (auto it = m_jobs.begin(); it != m_jobs.end();)
	{
		Job& job = *it;
		job.timeAccumulator += a_deltaSec;
		if (job.timeAccumulator > job.timeIntervalSec)
		{
			if (job.func(job.timeAccumulator, job.dataPtr) == EJobRepeat_NOREPEAT)
				it = m_jobs.erase(it);
			else
				++it;

			job.timeAccumulator -= job.timeIntervalSec;
		}
		else
		{
			++it;
		}
	}
}

void JobScheduler::addJob(float a_targetTimeIntervalSec, std::function<EJobRepeat(float, void*)> a_func, void* a_ptr)
{
	Job job;
	job.dataPtr = a_ptr;
	job.func = a_func;
	job.timeIntervalSec = a_targetTimeIntervalSec;
	job.timeAccumulator = 0.0f;
	m_jobs.push_back(job);
}