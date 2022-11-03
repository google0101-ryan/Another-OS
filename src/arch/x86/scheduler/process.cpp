#include <arch/x86/drivers/vga.hpp>
#include <arch/x86/scheduler/process.h>
#include <arch/x86/mem/virtmem.h>
#include <arch/x86/mem/heap.h>

static int currentPID = 1;
std::queue<Process*> ProcessHelper::processes;

ProcessHelper::ProcessHelper()
{}

Process* ProcessHelper::Create(char*, char*, bool)
{
	printf("Unimplemented creating process from file!\n");
	for(;;);
}

void* memcpy(void* dstptr, const void* srcptr, uint32_t size)
{
	unsigned char* dst = (unsigned char*) dstptr;
	const unsigned char* src = (const unsigned char*) srcptr;
	for (uint32_t i = 0; i < size; i++)
		dst[i] = src[i];
	return dstptr;
}

void* memset(void* buffer, char value, uint32_t size)
{
	unsigned char* buf = (unsigned char*)buffer;
	for (uint32_t i = 0; i < size; i++)
		buf[i] = (unsigned char)value;
	return buffer;
}

Process* ProcessHelper::CreateKernelProcess()
{
	Process* proc = new Process();
	memset(proc, 0, sizeof(Process));

	memcpy(proc->filename, "Kernel Process", 15);
	proc->id = currentPID++;
	proc->pageDirPhys = virt2phys((uint32_t)&boot_pt);
	proc->state = ProcessState::Active;
	proc->syscallID = 1;
	proc->heap.heapStart = KERNEL_HEAP_START;
	proc->heap.heapEnd = KERNEL_HEAP_SIZE;

	processes.push_back(proc);

	return proc;
}