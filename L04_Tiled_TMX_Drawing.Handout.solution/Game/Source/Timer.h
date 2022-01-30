#ifndef __TIMER_H__
#define __TIMER_H__

#include "Defs.h"

class Timer
{
public:

	// Constructor
	Timer();

	void Start();
	uint Read() const;
	float ReadSec() const;
	void Stop();
private:
	uint startTime;
	bool	running;
	uint	started_at;
	uint	stopped_at;
};

#endif //__TIMER_H__