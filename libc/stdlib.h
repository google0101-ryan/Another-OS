#pragma once

#include <syscall.h>

void exit(int errcode)
{
	__syscall1(SYSCALL_EXIT, errcode);
}