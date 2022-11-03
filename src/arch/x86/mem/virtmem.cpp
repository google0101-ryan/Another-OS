#include <arch/x86/mem/virtmem.h>
#include <arch/x86/mem/heap.h>

#include <stddef.h>

VirtualMem::PageTableEntry* VirtualMem::GetPageForAddress(uint32_t virtualAddress, bool shouldCreate, bool readWrite, bool userPages)
{
	uint32_t pageDirIndex = PAGEDIR_INDEX(virtualAddress);
	uint32_t pageTableIndex = PAGETBL_INDEX(virtualAddress);

	PageDirectory* pageDir = (PageDirectory*)PAGE_DIRECTORY_ADDRESS;

	if (pageDir->entries[pageDirIndex].present == 0 && shouldCreate)
	{
		void* pageTablePhys = phyMem->AllocateBlock();

		pageDir->entries[pageDirIndex].frame = (uint32_t)pageTablePhys / PAGE_SIZE;
		pageDir->entries[pageDirIndex].readWrite = readWrite;
		pageDir->entries[pageDirIndex].isUser = userPages;
		pageDir->entries[pageDirIndex].pageSize = FOUR_KB;
		pageDir->entries[pageDirIndex].present = 1;

		PageTable* pageTableVirt = (PageTable*)GetPageTableAddress(pageDirIndex);

		for (size_t i = 0; i < sizeof(PageTable); i++)
			((uint8_t*)pageTableVirt)[i] = 0;
		
		return &(pageTableVirt->entries[pageTableIndex]);
	}

	PageTable* pageTable = (PageTable*)GetPageTableAddress(pageDirIndex);
	return &pageTable->entries[pageTableIndex];
}

void VirtualMem::Init(PhysicalMem* phyMem)
{
	this->phyMem = phyMem;

	PageDirectory* pageDirectory = (PageDirectory*)&boot_pt;

	PageDirectoryEntry lastPDE;
	for (size_t i = 0; i < sizeof(PageDirectoryEntry); i++)
		((uint8_t*)&lastPDE)[i] = 0;
	
	lastPDE.frame = virt2phys((uint32_t)&boot_pt) / PAGE_SIZE;
	lastPDE.readWrite = 1;
	lastPDE.pageSize = FOUR_KB;
	lastPDE.present = 1;
	pageDirectory->entries[1023] = lastPDE;

	for (uint32_t i = KERNEL_HEAP_START; i < KERNEL_HEAP_START + KERNEL_HEAP_SIZE; i += PAGE_SIZE)
		AllocatePage(GetPageForAddress(i, true), true, true);

	pageDirectory->entries[0].frame = (uint32_t)phyMem->AllocateBlock();
	pageDirectory->entries[0].pageSize = FOUR_KB;
	pageDirectory->entries[0].present = 1;
	pageDirectory->entries[0].readWrite = 1;
	pageDirectory->entries[0].isUser = 1;

	PageTable* fourMB_PT = (PageTable*)GetPageTableAddress(0);
	for (size_t i = 0; i < sizeof(PageTable); i++)
		((uint8_t*)fourMB_PT)[i] = 0;
	
	for (uint16_t i = 1; i < 1024; i++)
	{
		fourMB_PT->entries[i].frame = i;
		fourMB_PT->entries[i].isUser = 0;
		fourMB_PT->entries[i].readWrite = 1;
		fourMB_PT->entries[i].present = 1;
	}

	for (uint32_t i = PAGE_SIZE; i < PhysicalMem::PageRoundUp(0x100000); i += PAGE_SIZE)
	{
		uint16_t index = PAGETBL_INDEX(i);
		fourMB_PT->entries[index].isUser = 1;
	}

	fourMB_PT->entries[0].frame = 0;
	fourMB_PT->entries[0].isUser = 1;
	fourMB_PT->entries[0].readWrite = 1;
	fourMB_PT->entries[0].present = 0;

	asm volatile("movl	%cr3,%eax");
	asm volatile("movl	%eax,%cr3");
}

void* VirtualMem::GetPageTableAddress(uint16_t pageTableNumber)
{
	uint32_t ret = PAGE_TABLE_ADDRESS;
	ret |= (pageTableNumber << PAGE_OFFSET_BITS);
	return (void*)ret;
}

void VirtualMem::AllocatePage(PageTableEntry* page, bool kernel, bool writeable)
{
	void* p = phyMem->AllocateBlock();
	if (!p)
		return;
	
	page->present = 1;
	page->readWrite = writeable ? 1 : 0;
	page->isUser = kernel ? 0 : 1;
	page->frame = (uint32_t)p / PAGE_SIZE;
}

void VirtualMem::MapVirtualToPhysical(void* phys, void* virt, bool kernel, bool writeable)
{
	PageTableEntry* page = GetPageForAddress((uint32_t)virt, true, writeable, !kernel);

	page->frame = (uint32_t)phys / PAGE_SIZE;
	page->isUser = kernel ? 0 : 1;
	page->readWrite = writeable ? 1 : 0;
	page->present = 1;

	asm volatile("invlpg (%0)" :: "r"(virt) : "memory");
}