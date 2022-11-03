#pragma once

#include <arch/x86/mem/phymem.h>

#include <stdint.h>

extern "C" void* boot_pt;

class VirtualMem
{
public:
	struct PageDirectoryEntry
	{
		uint32_t present : 1;
		uint32_t readWrite : 1;
		uint32_t isUser : 1;
		uint32_t writeThrough : 1;
		uint32_t canCache : 1;
		uint32_t accessed : 1;
		uint32_t reserved : 1;
		uint32_t pageSize : 1;
		uint32_t ignored : 1;
		uint32_t unused : 3;
		uint32_t frame : 20;
	} __attribute__((packed));

	struct PageTableEntry
	{
		uint32_t present : 1;
		uint32_t readWrite : 1;
		uint32_t isUser : 1;
		uint32_t writeThrough : 1;
		uint32_t canCache : 1;
		uint32_t accessed : 1;
		uint32_t dirty : 1;
		uint32_t reserved : 1;
		uint32_t global : 1;
		uint32_t unused : 3;
		uint32_t frame : 20;
	} __attribute__((packed));

	struct PageTable
	{
		PageTableEntry entries[1024];
	} __attribute__((packed));

	struct PageDirectory
	{
		PageDirectoryEntry entries[1024];
	} __attribute__((packed));
	
	enum PAGE_ENTRY_SIZE
	{
		FOUR_KB = 0,
		FOUR_MB = 1
	};
private:
	friend class Heap;

	#define USER_STACK_SIZE (32*1024)
	#define USER_STACK_TOP (KERNEL_VIRTUAL_OFFSET)
	#define USER_STACK (USER_STACK_TOP - USER_STACK_SIZE)

	#define PAGE_SIZE 4096
	#define KERNEL_PTNUM (KERNEL_VIRTUAL_OFFSET >> 22)
	#define PAGE_TABLE_ADDRESS 0xFFC00000
	#define PAGE_DIRECTORY_ADDRESS 0xFFFFF000

	#define PAGE_OFFSET_BITS 12

	#define PAGEDIR_INDEX(addr) (((uint32_t)addr) >> 22)	
    #define PAGETBL_INDEX(addr) ((((uint32_t)addr) >> 12) & 0x3ff)
    #define PAGEFRAME_INDEX(addr) (((uint32_t)addr) & 0xfff)

	void* GetPageTableAddress(uint16_t pageTableNumber);
	PageTableEntry* GetPageForAddress(uint32_t virtualAddress, bool shouldCreate, bool readWrite = true, bool userPages = false);

	PhysicalMem* phyMem;
public:
	void Init(PhysicalMem* phyMem);

	void AllocatePage(PageTableEntry* page, bool kernel, bool writeable);

	void MapVirtualToPhysical(void* physAddress, void* virtAddress, bool kernel, bool writable);

	void SwitchPageDirectory(uint32_t physAddr)
	{
		asm volatile("mov %0, %%cr3" :: "r"(physAddr));
	}
};