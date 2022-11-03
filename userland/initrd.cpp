#pragma once

#include <stdint.h>
#include <unistd.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>


#define PCI_BUS (bus,device,func,regs) (((bus)<<23)|((device)<<15)|((func)<<10)|(reg))

// Load some basic utilities to prepare the system for init
int main(int argc, char** argv)
{
	// Grab the pid of our process
	pid_t pid = getpid();

	// Request permission to access PCI bus I/O ports
	int res = ioperm(pid, 0xCF8, 4);

	if (res == -1)
	{
		printf("Failed to acquire I/O priveleges, halting\n");
		exit(1);
	}

	printf("Acquired IO permissions for PCI bus\n");
}