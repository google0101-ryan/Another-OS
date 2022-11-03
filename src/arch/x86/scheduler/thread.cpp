#include <arch/x86/scheduler/process.h>
#include <arch/x86/scheduler/thread.h>
#include <arch/x86/mem/heap.h>

ThreadHelper::ThreadHelper()
{}

extern void* memset(void*, char, uint32_t);

Thread* ThreadHelper::CreateFromFunction(void (*entryPoint)(), bool isKernel, uint32_t, Process* parent)
{
	Thread* result = new Thread();

	result->stack = (uint8_t*)Heap::malloc(THREAD_STACK_SIZE);
	memset(result->stack, 0, THREAD_STACK_SIZE);

	result->userStack = (parent == 0 ? (uint8_t*)USER_STACK : (uint8_t*)(USER_STACK - USER_STACK_SIZE * parent->threads.size()));
	result->userStackSize = USER_STACK_SIZE;

	result->regsPtr = (IDT::registers_t*)((uint32_t)result->stack + THREAD_STACK_SIZE - sizeof(IDT::registers_t));

	result->regsPtr->esp = (uint32_t)result->stack + THREAD_STACK_SIZE;

	result->regsPtr->eip = (uint32_t)entryPoint;

	result->regsPtr->cs = isKernel ? SEG_KERNEL_CODE : SEG_USER_CODE;
	result->regsPtr->ds = isKernel ? SEG_KERNEL_DATA : SEG_USER_DATA;
	result->regsPtr->ss = isKernel ? SEG_KERNEL_DATA : SEG_USER_DATA;

	result->state = ThreadState::Ready;

	result->parent = parent;

	return result;
}

void ThreadHelper::RemoveThread(Thread* thread)
{
	Heap::free(thread->stack);
	thread->state = ThreadState::Stopped;
}