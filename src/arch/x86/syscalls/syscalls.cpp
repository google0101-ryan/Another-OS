#include <arch/x86/syscalls/syscalls.h>

#include <arch/x86/drivers/vga.hpp>

syscall system_handlers[0x1000];

extern "C" uint32_t syscall_handler(uint32_t regs)
{
	printf("Unhandled system call\n");
	return regs;
}