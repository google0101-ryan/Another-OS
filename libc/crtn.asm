; ToAruOS User CRT0
BITS 32

section .init
	; .init goes here
	pop ebp
	ret

section .fini
	; .fini goes here
	pop ebp
	ret