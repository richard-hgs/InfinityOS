; ================================================
; entry.asm - Boot32-Barebones Kernel Entrypoint.
; ================================================
; Author: Infinity Technology
; Date: 12/14/2020
; ================================================

[bits 32]

[extern kernel_main]
[global _start]

_start:
	; Save the kernel sectors readed from ax to use later
	; mov [kernel_sectors_readed], ax

	pusha	;push all registers
	pushf	;push all flags

	; arg1 -> kernel_sectors_readed
	; mov ax, [kernel_sectors_readed]
	; push ax
	; call main kernel function
	call kernel_main

	popf	; pop all flags
	popa	; pop all registers
	ret		; return to calling function

kernel_sectors_readed resw 0x0000
