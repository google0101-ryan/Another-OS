#include <arch/x86/mem/phymem.h>
#include <arch/x86/drivers/vga.hpp>
#include <multiboot.h>

uint32_t memorySize = 0;
uint32_t usedBlocks = 0;
uint32_t maximumBlocks = 0;
uint32_t* memoryArray = 0;

uint32_t PhysicalMem::GetBitmapSize()
{
	return memorySize / BLOCK_SIZE / BLOCKS_PER_BYTE;
}

static inline void SetBit(uint32_t bit)
{
	memoryArray[bit / 32] |= (1 << (bit % 32));
}

static inline void UnsetBit(uint32_t bit)
{
	memoryArray[bit / 32] &= ~ (1 << (bit % 32));
}

void PhysicalMem::MarkFree(uint32_t base, uint32_t size)
{
	uint32_t align = base / BLOCK_SIZE;

	for (uint32_t blocks = size / BLOCK_SIZE; blocks > 0; blocks--)
	{
		UnsetBit(align++);
		usedBlocks--;
	}

	SetBit(0);
}

void PhysicalMem::MarkUsed(uint32_t base, uint32_t size)
{
	uint32_t align = base / BLOCK_SIZE;

	for (uint32_t blocks = size / BLOCK_SIZE; blocks > 0; blocks--)
	{
		SetBit(align++);
		usedBlocks++;
	}

	SetBit(0);
}



void PhysicalMem::Init(multiboot_info_t* mboot_ptr)
{
	uint32_t size = mboot_ptr->mem_upper * 1024;
	uint32_t bitmap = (uint32_t)&_kernel_end;

	printf("Memory Size: %d Mb\n", size / (1024*1024));
	printf("Bitmap: 0x%x\n", bitmap);

	memorySize = size;
	memoryArray = (uint32_t*)bitmap;
	maximumBlocks = size / BLOCK_SIZE;
	usedBlocks = maximumBlocks;

	for (uint32_t i = 0; i < usedBlocks / BLOCKS_PER_BYTE; i++)
	{
		memoryArray[i] = 0xFF;
	}

	printf("Bitmap size: %d Kb Bitmap end: 0x%x\n", GetBitmapSize() / 1024, bitmap + GetBitmapSize());

	printf("Parsing grub memory map\n");
	printf("Address of mmap: 0x%x\n", phys2virt(mboot_ptr->mmap_addr));

	grub_multiboot_memory_map_t* mmap = (grub_multiboot_memory_map_t*)phys2virt(mboot_ptr->mmap_addr);
	printf("-------------------------------------------------\n");
	while ((uint32_t)mmap < phys2virt(mboot_ptr->mmap_addr) + mboot_ptr->mmap_length)
	{
		printf("|->   0x%x   %d Kb      Type: %d", mmap->base_addr_low, mmap->length_low / 1024, mmap->type);

		if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE)
		{
			printf(" [Free]");
			MarkFree(mmap->base_addr_low, mmap->length_low);
		}
		printf("\n");

		mmap = (grub_multiboot_memory_map_t*)((uint32_t)mmap + mmap->size + sizeof(uint32_t));
	}
	printf("-------------------------------------------------\n");
}

void PhysicalMem::FreeBlocks(void* ptr, uint32_t size)
{
	uint32_t addr = (uint32_t)ptr;
	uint32_t frame = addr / BLOCK_SIZE;

	for (uint32_t i = 0; i < size; i++)
		UnsetBit(frame + i);
	
	usedBlocks -= size;
}

uint32_t PhysicalMem::FreeBlocks()
{
	return maximumBlocks - usedBlocks;
}

uint32_t PhysicalMem::FirstFree()
{
	for (uint32_t i = 0; i < maximumBlocks; i++)
	{
		if (memoryArray[i] != 0xffffffff)
			for (int j = 0; j < 32; j++)
			{
				int bit = 1 << j;
				if (!(memoryArray[i] & bit))
					return i * 4 * 8 + j;
			}
	}

	return -1;
}

void* PhysicalMem::AllocateBlock()
{
	if (FreeBlocks() <= 0)
		return 0;
	
	uint32_t frame = FirstFree();

	if (frame == (uint32_t)-1)
		return 0;
	
	SetBit(frame);

	uint32_t addr = frame * BLOCK_SIZE;
	usedBlocks++;

	return (void*)addr;
}