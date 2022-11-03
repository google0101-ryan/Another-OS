#pragma once

#include <stdint.h>
#include <syscall.h>


typedef int pid_t;

inline pid_t getpid()
{
	return __syscall0(0);
}