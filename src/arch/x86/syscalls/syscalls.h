#pragma once

#include <arch/x86/tables/idt.h>

typedef int (*syscall)(IDT::registers_t);

extern "C" uint32_t syscall_handler(uint32_t regs);