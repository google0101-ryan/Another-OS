#pragma once

#include <stdint.h>
#include <arch/x86/tables/idt.h>
#include <arch/x86/scheduler/thread.h>

class Scheduler
{
private:
	std::queue<Thread*> threadsList;
	Thread* currentThread = 0;

	uint32_t tickCount = 0;

	Thread* GetNextReadyThread();
	void ProcessSleepingThreads();

	bool switchForced = false;
public:
	bool enabled = true;
	Scheduler();

	uint32_t HandleInterrupt(uint32_t esp);

	void AddThread(Thread* thread, bool forceSwitch = false);
	void ForceSwitch();

	Thread* CurrentThread();
	Process* CurrentProcess();

	void InitialThreadUserJump(Thread* thread);

	void Block(Thread* thread, BlockedState reason = BlockedState::Unknown);
	void Unblock(Thread* thread, bool forceSwitch = false);
};