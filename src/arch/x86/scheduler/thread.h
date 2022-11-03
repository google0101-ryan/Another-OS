#pragma once

#include <queue.h>
#include <arch/x86/tables/idt.h>

#define THREAD_STACK_SIZE (1024*4)

#define SEG_USER_DATA 0x23
#define SEG_USER_CODE 0x1B
#define SEG_KERNEL_DATA 0x10
#define SEG_KERNEL_CODE 8

enum ThreadState
{
	Blocked,
	Ready,
	Stopped,
	Started
};

enum BlockedState
{
	Unknown,
	SleepMS,
	ReceiveIPC
};

struct Process;

struct Thread
{
	Process* parent;
	uint8_t* stack;
	uint8_t* userStack;
	uint32_t userStackSize;
	ThreadState state;
	BlockedState blockedState;
	IDT::registers_t* regsPtr;

	uint32_t timeDelta;
};

class ThreadHelper
{
private:
	ThreadHelper();
public:
	static Thread* CreateFromFunction(void (*entrypoint)(), bool isKernel = false, uint32_t flags = 0, Process* parent = nullptr);
	static void RemoveThread(Thread* thread);
};