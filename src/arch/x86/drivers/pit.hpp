#pragma once

#include <arch/x86/ports.h>
#include <arch/x86/scheduler/scheduler.h>

class PIT
{
public:
	void Init();

	void SetSched(Scheduler* sched);
};