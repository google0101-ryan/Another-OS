// Main header
#include <arch/x86/kernel/kernel.h>

// Drivers
#include <arch/x86/drivers/vga.hpp>

// Memory management
#include <arch/x86/mem/heap.h>

// Scheduling
#include <arch/x86/scheduler/scheduler.h>
#include <arch/x86/scheduler/process.h>

void PrintOK()
{
	console.set_cur_color(VGA_COLOR_GREEN, VGA_COLOR_BLACK);
	printf("[OK]\n");
	console.set_cur_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
}

uint32_t kernel_size = &_kernel_end - &_kernel_start;

struct initrd_header
{
	unsigned char magic; // The magic number is there to check for consistency.
	char name[64];
	unsigned int offset; // Offset in the initrd the file starts.
	unsigned int length; // Length of the file.
};

extern void* memcpy(void*, const void*, uint32_t);

void ArchKernel::Init(multiboot_info_t* info)
{
	// Initialize default 80x25 VGA console
	console.Init();
	
	
	printf("Console initialized...\t\t");
	PrintOK();

	// Install a GDT with entries for Kernel mode code and data
	// User-mode code and data, and a TSS for syscalls
	printf("Installing GDT...\t\t\t");
	gdt.Init();
	PrintOK();

	// Set up interrupts and remap PIC so we can begin
	// Handling interrupts and exceptions
	printf("Installing IDT...\t\t\t");
	idt.Init();
	PrintOK();

	// Set up the programmable interval timer
	// To interrupt us every 100ms
	printf("Setting up PIT...\t\t\t");
	pit.Init();
	PrintOK();

	// Set up the RAM allocator and mark the kernel
	// + all kernel modules (initrd) as used so they aren't overwritten
	phyMemManager.Init(info);
	phyMemManager.MarkUsed(0, PhysicalMem::PageRoundUp(0x100000 + kernel_size + phyMemManager.GetBitmapSize()));
	phyMemManager.MarkUsed(*(uint32_t*)phys2virt(info->mods_addr), *(uint32_t*)phys2virt(info->mods_addr + 4) - *(uint32_t*)phys2virt(info->mods_addr));

	// Initialize our page tables so we can
	// map and unmap memory at will
	// Maps the kernel heap and some vm86 stuff as well
	printf("Initializing virtual memory...\t");
	virtMemManager.Init(&phyMemManager);
	PrintOK();

	// Set up the heap now that it's mapped into memory
	printf("Initializing kernel heap...\t\t");
	Heap::Initialize(KERNEL_HEAP_START, KERNEL_HEAP_START + KERNEL_HEAP_SIZE, &virtMemManager);
	PrintOK();

	multiboot_info_t* mbi = (multiboot_info_t*)Heap::malloc(sizeof(multiboot_info_t));
	memcpy(mbi, info, sizeof(multiboot_info_t));

	console.set_cur_color(VGA_COLOR_MAGENTA, VGA_COLOR_BLACK);

	printf("\t- KERNEL CORE INITIALIZED -\n");

	console.set_cur_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);


	// Create the task scheduler and attach it to the pit,
	// so every 100ms we switch tasks
	Scheduler* sched = new Scheduler();

	pit.SetSched(sched);
	
	// Execute the fs/disk combo driver from initrd, then run it
	// It will load other drivers from disk before loading /sbin/init

	if (mbi->mods_count <= 0)
	{
		console.set_cur_color(VGA_COLOR_RED, VGA_COLOR_BLACK);
		printf("ERROR: couldn't load initrd, cannot continue\n");
		asm volatile("cli");
		for (;;);
	}
	
	

	for (;;);
}