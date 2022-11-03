bits 32

global _start
extern main
_start:
	call main
	mov edi, eax
	mov eax, 3
	int 0x80
_halt:
	jmp _halt