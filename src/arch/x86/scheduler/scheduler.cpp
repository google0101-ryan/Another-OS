#include <arch/x86/scheduler/scheduler.h>
#include <arch/x86/scheduler/thread.h>
#include <arch/x86/scheduler/process.h>
#include <arch/x86/tables/gdt.h>
#include <arch/x86/drivers/vga.hpp>
#include <arch/x86/mem/heap.h>
#include <arch/x86/kernel/kernel.h>

extern ArchKernel kernel;


extern "C" void enter_usermode(uint32_t location, uint32_t stackAddress, uint32_t flags);

Scheduler::Scheduler()
{
	this->currentThread = 0;
	this->threadsList.Clear();
	this->enabled = false;
	this->switchForced = false;
}

uint32_t Scheduler::HandleInterrupt(uint32_t esp)
{
	tickCount++;
	if (this->switchForced == false)
		ProcessSleepingThreads();
	else
		this->switchForced = false;
	
	if (tickCount == 30)
	{
		tickCount = 0;

		if (threadsList.size() > 0 && this->enabled)
		{
			Thread* nextThread = GetNextReadyThread();

			while (nextThread->state == Stopped)
			{
				printf("Removing thread %x from system\n", (uint32_t)nextThread);
				threadsList.Remove(nextThread);
				delete nextThread;

				nextThread = GetNextReadyThread();
			}

			if (currentThread != 0 && currentThread->state == Stopped)
			{
				printf("Removing thread %x from system\n", (uint32_t)currentThread);
				threadsList.Remove(currentThread);
				delete currentThread;
			}
			else if (esp >= KERNEL_HEAP_START)
			{
				currentThread->regsPtr = (IDT::registers_t*)esp;
			}

			currentThread = nextThread;

			if (nextThread->state == Started && nextThread->parent->isUserspace)
			{
				nextThread->state = ThreadState::Ready;

				InitialThreadUserJump(nextThread);
			}

			esp = (uint32_t)nextThread->regsPtr;

			if (nextThread->parent && nextThread->parent->pageDirPhys != 0)
				kernel.GetVirtMem().SwitchPageDirectory(nextThread->parent->pageDirPhys);
			
			kernel.GetGDT().SetTSS(0x10, (uint32_t)nextThread->stack + THREAD_STACK_SIZE);
		}
	}

	return esp;
}

Thread* Scheduler::GetNextReadyThread()
{
	int currentThreadIndex = (currentThread != 0 ? threadsList.IndexOf(currentThread) : 0);
	currentThreadIndex += 1;
	if (currentThreadIndex >= threadsList.size())
		currentThreadIndex = 0;
	
	while (threadsList[currentThreadIndex]->state == Blocked)
	{
		currentThreadIndex += 1;
		if (currentThreadIndex >= threadsList.size())
			currentThreadIndex = 0;
	}

	return threadsList[currentThreadIndex];
}

void Scheduler::AddThread(Thread* thread, bool forceSwitch)
{
	threadsList.push_back(thread);

	if (forceSwitch)
	{
		if (thread->parent && thread->parent->isUserspace)
			InitialThreadUserJump(thread);
		
		this->ForceSwitch();
	}
}

void Scheduler::InitialThreadUserJump(Thread* thread)
{
	asm volatile("cli");

	kernel.GetGDT().SetTSS(0x10, (uint32_t)thread->stack + THREAD_STACK_SIZE);

	kernel.GetVirtMem().SwitchPageDirectory(thread->parent->pageDirPhys);

	currentThread = thread;

	this->enabled = true;

	outb(0x20, 0x20);

	enter_usermode(thread->regsPtr->eip, (uint32_t)thread->userStack + thread->userStackSize, thread->regsPtr->eflags);
}

void Scheduler::ForceSwitch()
{
	this->switchForced = true;
	this->enabled = true;
	this->tickCount = 29;
	asm volatile("int $0x20");
}

void Scheduler::Unblock(Thread* thread, bool forceSwitch)
{
	thread->state = ThreadState::Ready;

	if (forceSwitch)
		ForceSwitch();
}

void Scheduler::ProcessSleepingThreads()
{
	for (int i = 0; i < threadsList.size(); i++)
	{
		Thread* thread = threadsList[i];
		if (thread->state == Blocked && thread->blockedState == SleepMS && thread->timeDelta > 0)
		{
			thread->timeDelta--;
			if (thread->timeDelta <= 0)
				Unblock(thread);
		}
	}
}