#include <arch/x86/drivers/pit.hpp>
#include <arch/x86/drivers/vga.hpp>
#include <arch/x86/tables/idt.h>

static uint32_t ticks = 0;
static int index = 0;

static char ticker[] =
{
	'/',
	'-',
	'\\',
	'|'
};

Scheduler* scheduler;

static uint32_t pit_handler(uint32_t esp)
{
	ticks++;
	int x = console.GetX();
	int y = console.GetY();

	console.GoToXY(79, 0);
	printf("%c", ticker[index++]);
	if (index > 3)
		index = 0;
	console.GoToXY(x, y);

	if (scheduler)	
		return scheduler->HandleInterrupt(esp);
	else
		return esp;
}

void PIT::Init()
{
	IDT::InstallHandler(32, pit_handler);

	uint32_t divisor = 1193180 / 100;

	outb(0x43, 0x36);

	uint8_t l = (uint8_t)(divisor & 0xFF);
	uint8_t h = (uint8_t)(divisor >> 8) & 0xFF;

	outb(0x40, l);
	outb(0x40, h);
}

void PIT::SetSched(Scheduler* s)
{
	scheduler = s;
}