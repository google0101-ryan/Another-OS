#include <kernel/kernel.h>

#include <arch/x86/tables/gdt.h>
#include <arch/x86/tables/idt.h>

#include <arch/x86/drivers/pit.hpp>

#include <arch/x86/mem/phymem.h>
#include <arch/x86/mem/virtmem.h>

#include <multiboot.h>

class ArchKernel
{
private:
	GDT gdt;
	IDT idt;
	PIT pit;
	PhysicalMem phyMemManager;
	VirtualMem virtMemManager;
public:
	void Init(multiboot_info_t* info);

	PhysicalMem& GetPhysMem() {return phyMemManager;}
	VirtualMem& GetVirtMem() {return virtMemManager;}

	GDT& GetGDT() {return gdt;}
};