#pragma once

#include <syscall.h>
#include <unistd.h>

int ioperm(pid_t _pid, uint16_t _p, uint8_t _s)
{
	return __syscall3(SYSCALL_IOPERM, _pid, _p, _s);
}