#include <arch/x86/mem/heap.h>
#include <arch/x86/mem/phymem.h>
#include <arch/x86/mem/virtmem.h>

#include <arch/x86/drivers/vga.hpp>

uint32_t Heap::startAddress = 0;
uint32_t Heap::endAddress = 0;
Heap::MemoryHeader* Heap::firstHeader = 0;
VirtualMem* Heap::virtMem = 0;

void Heap::Initialize(uint32_t start, uint32_t end, VirtualMem* virtMe)
{
	Heap::virtMem = virtMe;

	if (start % PAGE_SIZE != 0 || end % PAGE_SIZE != 0)
	{
		printf("Heap: Start or end is not page-aligned\n");
		for(;;);
	}

	Heap::startAddress = start;
	Heap::endAddress = end;

	firstHeader = (MemoryHeader*)start;
	firstHeader->allocated = false;
	firstHeader->prev = nullptr;
	firstHeader->next = nullptr;
	firstHeader->size = end - start - sizeof(MemoryHeader);
	firstHeader->magic = MEMORY_HEADER_MAGIC;
}

Heap::MemoryHeader* Heap::FirstFree(uint32_t size)
{
	MemoryHeader* block = firstHeader;
	while (block && block->magic == MEMORY_HEADER_MAGIC)
	{
		if (block->allocated || block->size < size)
		{
			block = block->next;
			continue;
		}

		return block;
	}
	return nullptr;
}

void* Heap::InternalAllocate(uint32_t size)
{
	size = align_up(size, sizeof(uint32_t));

	MemoryHeader* freeBlock = FirstFree(size);

	if (freeBlock == 0)
	{
		printf("PANIC: Out of heap space!\n");
		for(;;);
	}

	if (freeBlock->size >= (size + sizeof(MemoryHeader) + MINIMAL_SPLIT_SIZE))
	{
		MemoryHeader* nextBlock = (MemoryHeader*)((uint32_t)freeBlock + size + sizeof(MemoryHeader));

		nextBlock->allocated = false;
		nextBlock->size = freeBlock->size - size - sizeof(MemoryHeader);
		nextBlock->prev = freeBlock;
		nextBlock->next = freeBlock->next;
		if (freeBlock->next != 0)
			freeBlock->next->prev = nextBlock;
		
		nextBlock->magic = MEMORY_HEADER_MAGIC;

		freeBlock->next = nextBlock;
	}

	freeBlock->allocated = true;
	freeBlock->size = size;

	return (void*)((uint32_t)freeBlock + sizeof(MemoryHeader));
}

void Heap::free(void* ptr)
{
	MemoryHeader* block = (MemoryHeader*)((uint32_t)ptr - sizeof(MemoryHeader));

	if (block->magic != MEMORY_HEADER_MAGIC || !block->allocated)
	{
		printf("ERROR: Double free detected!\n");
		for (;;);
	}

	block->allocated = false;

	if (block->prev && !block->prev->allocated)
	{
		block->prev->next = block->next;
		block->prev->size += block->size + sizeof(MemoryHeader);
		
		if (block->next)
			block->next->prev = block->prev;
		
		block = block->prev;
	}

	if (block->next && !block->next->allocated)
	{
		block->size += block->next->size + sizeof(MemoryHeader);

		block->next = block->next->next;
		
		if (block->next)
			block->next->prev = block;
	}
}

void* Heap::malloc(uint32_t size, uint32_t* physReturn)
{
	void* addr = InternalAllocate(size);
	if (physReturn != 0)
	{
		VirtualMem::PageTableEntry* page = virtMem->GetPageForAddress((uint32_t)addr, 0);
		*physReturn = (page->frame * PAGE_SIZE) + ((uint32_t)addr & 0xFFF);
	}
	return addr;
}


void* operator new(size_t size)
{
	return Heap::malloc(size);
}

void* operator new[](size_t size)
{
	return Heap::malloc(size);
}

void* operator new(size_t, void* ptr)
{
	return ptr;
}

void* operator new[](size_t, void* ptr)
{
    return ptr;
}
 
void operator delete(void *p)
{
    Heap::free(p);
}
 
void operator delete[](void *p)
{
    Heap::free(p);
}

void operator delete(void* ptr, size_t)
{
    Heap::free(ptr);
}
void operator delete[](void* ptr, size_t)
{
    Heap::free(ptr);
}