#pragma once

#include <stdint.h>
#include <stddef.h>

#include <arch/x86/mem/phymem.h>
#include <arch/x86/mem/virtmem.h>

#define KERNEL_HEAP_START (KERNEL_VIRTUAL_OFFSET + (4*1024*1024))
#define KERNEL_HEAP_SIZE (16*1024*1024)

#define MINIMAL_SPLIT_SIZE 4

#define MEMORY_HEADER_MAGIC 1234567890

#ifndef align_up
#define align_up(num, align) \
	(((num) + ((align) - 1)) & ~((align) - 1))
#endif

class Heap
{
private:
	struct MemoryHeader
	{
		uint32_t magic;

		MemoryHeader* next;
		MemoryHeader* prev;
		uint32_t allocated;
		uint32_t size;
	} __attribute__((packed));

	static uint32_t startAddress;
	static uint32_t endAddress;

	static MemoryHeader* firstHeader;

	static void* InternalAllocate(uint32_t size);
	static MemoryHeader* FirstFree(uint32_t size);

	static VirtualMem* virtMem;

public:
	static void Initialize(uint32_t start, uint32_t end, VirtualMem* virtMem);

	static void* malloc(uint32_t size, uint32_t* physReturn = nullptr);
	static void free(void* ptr);

	static void* alignedMalloc(uint32_t size, uint32_t align, uint32_t* physReturn = 0);
    static void allignedFree(void* ptr);

	static bool CheckForErrors();
	static uint32_t UsedMemory();
};