section .multiboot.data

MBOOT_MAGIC equ 0x1BADB002
MBOOT_MEM equ 1 << 1
MBOOT_ALIGN equ 1 << 0
MBOOT_FLAGS equ MBOOT_MEM | MBOOT_ALIGN

mboot_header:
	dd MBOOT_MAGIC
	dd MBOOT_FLAGS
	dd -(MBOOT_MAGIC + MBOOT_FLAGS)

KERNEL_ADDR equ 0xC0000000
KERNEL_PAGE_NUM equ (KERNEL_ADDR >> 22)

section .data
align 4096
global boot_pt
boot_pt:
	dd 0x83
	times (KERNEL_PAGE_NUM - 1) dd 0
	dd 0x83
	times (1024 - KERNEL_PAGE_NUM) dd 0

section .text
global _start
extern kmain
_start:
	cli

	mov ecx, (boot_pt - KERNEL_ADDR)
	mov cr3, ecx

	mov ecx, cr4
	or ecx, 0x10
	mov cr4, ecx

	mov ecx, cr0
	or ecx, 0x80000000
	mov cr0, ecx

	lea ecx, [higher_half]
	jmp ecx

higher_half:
	mov esp, _stackStart

	push 0
	popf

	add ebx, KERNEL_ADDR
	
	push eax
	push ebx

	call kmain

	cli
	hlt

section .bss
align 32
_stackEnd:
	resb 8192
_stackStart: