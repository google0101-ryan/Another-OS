#pragma once

#include <stdint.h>

#define SYSCALL_GETPID 0
#define SYSCALL_IOPERM 1
#define SYSCALL_WRITE  2
#define SYSCALL_EXIT   3

static inline uint32_t __syscall0(long n)
{
	unsigned long ret;
	__asm__ __volatile__("int $0x80" : "=a"(ret) : "a"(n) : "ecx", "memory");
	return ret;
}

static inline uint32_t __syscall1(long n, long a1)
{
	unsigned long ret;
	__asm__ __volatile__("int $0x80" : "=a"(ret) : "a"(n), "D"(a1) : "ecx", "memory");
	return ret;
}

static inline long __syscall3(long n, long a1, long a2, long a3)
{
	unsigned long ret;
	__asm__ __volatile__ ("int $0x80" : "=a"(ret) : "a"(n), "D"(a1), "S"(a2),
						  "d"(a3) : "ecx", "memory");
	return ret;
}