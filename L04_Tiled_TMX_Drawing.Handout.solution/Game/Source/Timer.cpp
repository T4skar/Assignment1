// ----------------------------------------------------
// Fast timer with milisecons precision
// ----------------------------------------------------
#include "Defs.h"
#include "Timer.h"
#include "SDL\include\SDL_timer.h"

// L07: DONE 1: Fill Start(), Read(), ReadSec() methods
// they are simple, one line each!
	
Timer::Timer()
{
	Start();
}

void Timer::Start()
{
	running = true;
	started_at = SDL_GetTicks();
}

uint Timer::Read() const
{
	if (running == true)
	{
		return SDL_GetTicks() - started_at;
	}
	else
	{
		return stopped_at - started_at;
	}
}
float Timer::ReadSec() const
{
	return (float)(SDL_GetTicks() - startTime) / 1000.0f;
}

void Timer::Stop()
{
	running = false;
	stopped_at = SDL_GetTicks();
}

// ---------------------------------------------
