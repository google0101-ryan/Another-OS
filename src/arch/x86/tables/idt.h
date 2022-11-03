#pragma once

#include <stdint.h>

class IDT
{
public:
	typedef struct registers
	{
		uint32_t ds;                  // Data segment selector
		uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; // Pushed by pusha.
		uint32_t int_no, err_code;    // Interrupt number and error code (if applicable)
		uint32_t eip, cs, eflags, useresp, ss; // Pushed by the processor automatically.
	} registers_t;

	typedef uint32_t (*irq)(uint32_t);

	void Init();
	void RemapIRQs(); // By default, the PIC is mapped over top of exceptions. Fix it
	static void AckInt(int i);
	static void InstallHandler(int i, irq handler, bool isHw = true);
};