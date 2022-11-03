#pragma once

#include <stdint.h>

#include <multiboot.h>

extern "C" uint32_t _kernel_end;
extern "C" uint32_t _kernel_start;

#define KERNEL_VIRTUAL_OFFSET 0xC0000000

#define phys2virt(x) ((x) + KERNEL_VIRTUAL_OFFSET)
#define virt2phys(x) ((x) - KERNEL_VIRTUAL_OFFSET)

#define BLOCK_SIZE (4*1024)
#define BLOCKS_PER_BYTE 8

class PhysicalMem
{
private:
	typedef struct multiboot_memory_map 
	{
            unsigned int size;
            unsigned long base_addr_low;
            unsigned long base_addr_high;
            unsigned long length_low;
            unsigned long length_high;
            unsigned int type;
    }  __attribute__((packed)) grub_multiboot_memory_map_t;

	void FreeBlocks(void* ptr, uint32_t size);
	uint32_t FreeBlocks();
	uint32_t FirstFree();
public:
	void Init(multiboot_info_t* mboot_ptr);

	uint32_t GetBitmapSize();

	void MarkUsed(uint32_t base, uint32_t size);
	void MarkFree(uint32_t base, uint32_t size);

	void* AllocateBlock();

	static inline uint32_t PageRoundUp(uint32_t address)
	{
		if ((address & 0xFFFFF000) != address)
		{
			address &= 0xFFFFF000;
			address += 0x1000;
		}
		return address;
	}
};