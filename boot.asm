; This is the file the contains the Multiboot header and the entry point for the kernel

; Multiboot header
section .multiboot
align 4
	; Magic number
	dd 0x1BADB002
	; Flags
	dd 0x00
	; Checksum
	dd - (0x1BADB002 + 0x00)


; Kernel entry point
section .text
global start
extern kmain
extern kbd_handler_main
extern fault_handler
extern context_switch
extern timer_handler_main

; Loads the IDT
global load_idt
load_idt:
	mov edx, [esp + 4]
	lidt [edx]
	ret

global kbd_handler
kbd_handler:
	pusha
	call kbd_handler_main
	popa
	iretd

; Handles any interrupts that are not specifically handled
global isr_stub:
isr_stub:
	pusha
	call fault_handler
	popa
	iretd

global timer_handler
timer_handler:
	pusha
	call timer_handler_main
	popa
	iretd

global context_switch

start:
	; Disable interrupts
	cli
	; Set up stack
	mov esp, stack_space

	; Push Multiboot magic number and info struct onto stack
	push eax
	push ebx

	; Call the entry point for the kernel
	call kmain
	; Halt CPU
	hlt

; Stack
section .bss
; 8KB
resb 8192

stack_space:
