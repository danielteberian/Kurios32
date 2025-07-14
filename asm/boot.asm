; This is the file the contains the Multiboot header and the entry point for the kernel

; Set flags so that the kernel can gather information about video mode
%define MULTIBOOT_HEADER_FLAGS 0x00000003


; Multiboot header
section .multiboot
align 4
	; Magic number
	dd 0x1BADB002
	; Flags
	dd MULTIBOOT_HEADER_FLAGS
	; Checksum
	dd - (0x1BADB002 + MULTIBOOT_HEADER_FLAGS)


	; header_addr
	; dd 0

	; load_address
	; dd 0

	; load_end_address
	; dd 0

	; bss_end_addr
	; dd 0

	; entry_addr
	; dd 0

	; mode_type
	; dd 0

	; Width
	; dd 1024

	; Height
	; dd 768

	; Color depth
	; dd 32


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
