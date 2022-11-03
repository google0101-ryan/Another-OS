#include <arch/x86/tables/idt.h>
#include <arch/x86/drivers/vga.hpp>
#include <arch/x86/ports.h>
#include <arch/x86/syscalls/syscalls.h>

struct idt_entry
{
	uint16_t offset0;
	uint16_t selector;
	uint8_t zero;
	uint8_t type_attr;
	uint16_t offset1;
} __attribute__((packed));

struct idt_descriptor
{
	uint16_t limit;
	uint32_t base;
} __attribute__((packed));

#define ISR(num) extern "C" void isr##num();

ISR(0);
ISR(1);
ISR(2);
ISR(3);
ISR(4);
ISR(5);
ISR(6);
ISR(7);
ISR(8);
ISR(9);
ISR(10);
ISR(11);
ISR(12);
ISR(13);
ISR(14);
ISR(15);
ISR(16);
ISR(17);
ISR(18);
ISR(19);
ISR(20);
ISR(21);
ISR(22);
ISR(23);
ISR(24);
ISR(25);
ISR(26);
ISR(27);
ISR(28);
ISR(29);
ISR(30);
ISR(31);

#define IRQ(num) extern "C" void irq##num();

IRQ(0);
IRQ(1);
IRQ(2);
IRQ(3);
IRQ(4);
IRQ(5);
IRQ(6);
IRQ(7);
IRQ(8);
IRQ(9);
IRQ(10);
IRQ(11);
IRQ(12);
IRQ(13);
IRQ(14);
IRQ(15);
IRQ(128);

static idt_entry idt[256];
static idt_descriptor idt_ptr;

extern "C" void int_handler(IDT::registers_t regs)
{
	printf("Received exception %d\n", regs.int_no);
	for (;;);
}

IDT::irq handlers[256];

extern "C" uint32_t irq_handler(uint32_t esp)
{
	IDT::registers_t* regs = (IDT::registers_t*)esp;

	IDT::AckInt(regs->int_no);

	if (!handlers[regs->int_no])
	{
		printf("Unhandled IRQ %d\n", regs->int_no);
		for (;;);
	}

	return handlers[regs->int_no](esp);
}

void idt_create_entry(int idx, uint32_t base, uint16_t sel, uint8_t flags)
{
	idt[idx].offset0 = base & 0xFFFF;
	idt[idx].offset1 = ((base) >> 16) & 0xFFFF;
	idt[idx].selector = sel;
	idt[idx].type_attr = flags;
	idt[idx].zero = 0;
}

void IDT::Init()
{
	idt_create_entry(0, (uint32_t)isr0, 0x08, 0x8E);
	idt_create_entry(1, (uint32_t)isr1, 0x08, 0x8E);
	idt_create_entry(2, (uint32_t)isr2, 0x08, 0x8E);
	idt_create_entry(3, (uint32_t)isr3, 0x08, 0x8E);
	idt_create_entry(4, (uint32_t)isr4, 0x08, 0x8E);
	idt_create_entry(5, (uint32_t)isr5, 0x08, 0x8E);
	idt_create_entry(6, (uint32_t)isr6, 0x08, 0x8E);
	idt_create_entry(7, (uint32_t)isr7, 0x08, 0x8E);
	idt_create_entry(8, (uint32_t)isr8, 0x08, 0x8E);
	idt_create_entry(9, (uint32_t)isr9, 0x08, 0x8E);
	idt_create_entry(10, (uint32_t)isr10, 0x08, 0x8E);
	idt_create_entry(11, (uint32_t)isr11, 0x08, 0x8E);
	idt_create_entry(12, (uint32_t)isr12, 0x08, 0x8E);
	idt_create_entry(13, (uint32_t)isr13, 0x08, 0x8E);
	idt_create_entry(14, (uint32_t)isr14, 0x08, 0x8E);
	idt_create_entry(15, (uint32_t)isr15, 0x08, 0x8E);
	idt_create_entry(16, (uint32_t)isr16, 0x08, 0x8E);
	idt_create_entry(17, (uint32_t)isr17, 0x08, 0x8E);
	idt_create_entry(18, (uint32_t)isr18, 0x08, 0x8E);
	idt_create_entry(19, (uint32_t)isr19, 0x08, 0x8E);
	idt_create_entry(20, (uint32_t)isr20, 0x08, 0x8E);
	idt_create_entry(21, (uint32_t)isr21, 0x08, 0x8E);
	idt_create_entry(22, (uint32_t)isr22, 0x08, 0x8E);
	idt_create_entry(23, (uint32_t)isr23, 0x08, 0x8E);
	idt_create_entry(24, (uint32_t)isr24, 0x08, 0x8E);
	idt_create_entry(25, (uint32_t)isr25, 0x08, 0x8E);
	idt_create_entry(26, (uint32_t)isr26, 0x08, 0x8E);
	idt_create_entry(27, (uint32_t)isr27, 0x08, 0x8E);
	idt_create_entry(28, (uint32_t)isr28, 0x08, 0x8E);
	idt_create_entry(29, (uint32_t)isr29, 0x08, 0x8E);
	idt_create_entry(30, (uint32_t)isr30, 0x08, 0x8E);
	idt_create_entry(31, (uint32_t)isr31, 0x08, 0x8E);

	RemapIRQs();

	idt_create_entry(32, (uint32_t)irq0, 0x08, 0x8E);
	idt_create_entry(33, (uint32_t)irq1, 0x08, 0x8E);
	idt_create_entry(34, (uint32_t)irq2, 0x08, 0x8E);
	idt_create_entry(35, (uint32_t)irq3, 0x08, 0x8E);
	idt_create_entry(36, (uint32_t)irq4, 0x08, 0x8E);
	idt_create_entry(37, (uint32_t)irq5, 0x08, 0x8E);
	idt_create_entry(38, (uint32_t)irq6, 0x08, 0x8E);
	idt_create_entry(39, (uint32_t)irq7, 0x08, 0x8E);
	idt_create_entry(40, (uint32_t)irq8, 0x08, 0x8E);
	idt_create_entry(41, (uint32_t)irq9, 0x08, 0x8E);
	idt_create_entry(42, (uint32_t)irq10, 0x08, 0x8E);
	idt_create_entry(43, (uint32_t)irq11, 0x08, 0x8E);
	idt_create_entry(44, (uint32_t)irq12, 0x08, 0x8E);
	idt_create_entry(45, (uint32_t)irq13, 0x08, 0x8E);
	idt_create_entry(46, (uint32_t)irq14, 0x08, 0x8E);
	idt_create_entry(47, (uint32_t)irq15, 0x08, 0x8E);

	idt_create_entry(128, (uint32_t)irq128, 0x08, 0xEE);

	idt_ptr.base = (uint32_t)&idt;
	idt_ptr.limit = sizeof(idt_entry) * 256 - 1;

	asm("lidt (%0)" :: "r"(&idt_ptr));
	asm("sti");

	InstallHandler(0x80, syscall_handler);
}

void IDT::RemapIRQs()
{
	outb(0x20, 0x11);
	outb(0xA0, 0x11);
	outb(0x21, 0x20);
	outb(0xA1, 0x28);
	outb(0x21, 0x04);
	outb(0xA1, 0x02);
	outb(0x21, 0x01);
	outb(0xA1, 0x01);
	outb(0x21, 0x0);
	outb(0xA1, 0x0);

	outb(0x21, 0xff);
	outb(0xA1, 0xff); // Mask all interrupts, as we don't currently have handlers for them
}

void IDT::AckInt(int i)
{
	if (i >= 8)
		outb(0xA0, 0x20);
	outb(0x20, 0x20);
}

void IDT::InstallHandler(int i, irq handler, bool isHw)
{
	handlers[i] = handler;

	if (isHw)
	{
		uint16_t port;
		uint8_t value;

		i -= 32;

		if (i < 8)
			port = 0x21;
		else
		{
			port = 0xA1;
			i -= 8;
		}
		
		value = inb(port) & ~(1 << i);
		outb(port, value);
	}
}