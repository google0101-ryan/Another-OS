#pragma once

#include <arch/x86/scheduler/thread.h>
#include <queue.h>

enum ProcessState
{
	Active
};

#define PROC_USER_HEAP_SIZE (1024*1024)

struct Process
{
	int id;
	int syscallID;
	bool isUserspace;
	char* args;
	ProcessState state;
	std::queue<Thread*> threads;
	uint32_t pageDirPhys;
	struct Executable
	{
		uint32_t memBase;
		uint32_t memSize;
	} executable;

	struct ProcHeap
	{
		uint32_t heapStart;
		uint32_t heapEnd;
	} heap;

	char filename[32];
};

class ProcessHelper
{
private:
	ProcessHelper();
public:
	static std::queue<Process*> processes;

	static Process* Create(char* fileName, char* arguments = 0, bool isKernel = false);
    static Process* CreateKernelProcess();
    static void RemoveProcess(Process* proc);
    static void UpdateHeap(Process* proc, uint32_t newEndAddr);
    static Process* ProcessById(int id);
};