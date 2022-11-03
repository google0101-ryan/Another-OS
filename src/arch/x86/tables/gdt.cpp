#include <arch/x86/tables/gdt.h>
#include <arch/x86/drivers/vga.hpp>

struct gdt_decriptor
{
	uint16_t size;
	uint32_t offset;
} __attribute__((packed));

struct gdt_entry
{
	uint16_t limit0;
	uint16_t base0;
	uint16_t base1: 8, type: 4, s: 1, dpl: 2, p: 1;
	uint16_t limit1: 4, avl: 1, l: 1, d: 1, g: 1, base2: 8;
} __attribute__((packed));

struct tss_entry
{
	uint32_t prev_tss;
	uint32_t esp0;
	uint32_t ss0;
	uint32_t esp1;
	uint32_t ss1;
	uint32_t esp2;
	uint32_t ss2;
	uint32_t cr3;
	uint32_t eip;
	uint32_t eflags;
	uint32_t eax;
	uint32_t ecx;
	uint32_t edx;
	uint32_t ebx;
	uint32_t esp;
	uint32_t ebp;
	uint32_t esi;
	uint32_t edi;
	uint32_t es;
	uint32_t cs;
	uint32_t ss;
	uint32_t ds;
	uint32_t fs;
	uint32_t gs;
	uint32_t ldt;
	uint16_t trap;
	uint16_t iomap_base;
} __attribute__((packed));

static gdt_decriptor descriptor;
static tss_entry tss;
static gdt_entry gdt[6];

void GDT::CreateEntry(int idx, uint32_t base, uint32_t limit, int flags)
{
	gdt_entry* entry = &gdt[idx];

	entry->limit0 = (uint16_t)(limit);
	entry->limit1 = ((limit) >> 16) & 0x0F;
	entry->base0 = (uint16_t)(base);
	entry->base1 = ((base) >> 16) & 0xFF;
	entry->base2 = ((base) >> 24) & 0xFF;
	entry->type = (flags & 0x0f);
	entry->s = (flags >> 4) & 0x01;
	entry->dpl = (flags >> 5) & 0x03;
	entry->p = (flags >> 7) & 0x01;
	entry->avl = (flags >> 12) & 0x01;
	entry->l = (flags >> 13) & 0x01;
	entry->d = (flags >> 14) & 0x01;
	entry->g = (flags >> 15) & 0x01;
}

void GDT::Init()
{
	CreateEntry(0, 0, 0, 0);
	CreateEntry(1, 0, 0xfffff, 0xc09a);
	CreateEntry(2, 0, 0xfffff, 0xc092);
	CreateEntry(3, 0, 0xfffff, 0xc0fa);
	CreateEntry(4, 0, 0xfffff, 0xc0f2);

	uint32_t tss_base = (uint32_t)&tss;
	uint32_t tss_limit = tss_base + sizeof(tss_entry);

	CreateEntry(5, tss_base, tss_limit, 0x89);

	tss.ss0 = 0x10;
	tss.esp0 = 0;

	tss.cs = 0x0b;
	tss.ss = tss.ds = tss.es = tss.fs = tss.gs = 0x13;

	CommitGdt();
}

void GDT::SetTSS(uint32_t ss0, uint32_t esp0)
{
	tss.ss0 = ss0;
	tss.esp0 = esp0;
}

__attribute__((noinline)) void GDT::CommitGdt()
{
	descriptor.size = sizeof(gdt_entry) * 6 - 1;
	descriptor.offset = (uint32_t)(void*)&gdt;

	__asm__("lgdt (%0)" :: "r"(&descriptor));
	__asm__("movl %0, %%ds" :: "a"(0x10));
	__asm__("movl %0, %%es" :: "a"(0x10));
	__asm__("movl %0, %%fs" :: "a"(0x10));
	__asm__("movl %0, %%gs" :: "a"(0x10));
	__asm__("movl %0, %%ss" :: "a"(0x10));
	__asm__("jmp $0x8, $gdt_commit_end");
	__asm__("gdt_commit_end:");
	__asm__("movl $0x2B, %eax");
	__asm__("ltr %ax");
}