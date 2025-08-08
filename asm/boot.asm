; This file contains the basic code needed to set up paging and load the kernel correctly


PAGE_PRESENT equ 1
PAGE_WRITE equ 2


section.multiboot
align 4
	dd 0x1BADB002
	dd 0x00
	dd - (0x1BADB002 + 0x00)


section.text
global start
extern kmain


start:
	cli

	; Store the MB info pointer
	mov [mb_ptr], ebx
	mov [magic_num], eax

	; Set up the higher-half paging
	; First, set up a page table
	mov edi, temp_page_tab
	mov ecx, 1024
	xor eax, eax
	; Zero out the page table
	rep stosd


	; ID map first 4MB of physical memory
	mov edi, temp_page_tab
	mov ecx, 1024
	mov eax, (PAGE_PRESENT | PAGE_WRITE)

.map_low_mem:
	stosd
	add eax, 4096
	loop.map_low_mem

	; Create page directory
	mov edi, temp_page_dir
	mov ecx, 1024
	xor eax, eax
	; Zero out page directory
	rep stosd


	; Map page table at virtual address 0x0/0xC0000000
	mov eax, temp_page_tab
	or eax, (PAGE_PRESENT | PAGE_WRITE)


	; Map first entry
	mov [temp_page_tab], eax

	; Map 768th entry (higher-half)
	mov [temp_page_dir + 768 * 4], eax

	; Load page directory, enable paging
	mov eax, temp_page_dir
	mov cr3, eax
	mov eax, cr0
	; Set the PG bit
	or eax, 0x80000000
	mov cr0, eax

	; Jump to higher-half label, force EIP to high virtual address
	lea eax, [higher_half]
	jmp eax


higher_half:

	; Set up stack at known location with high memory
	mov esp, stack_space + 4096

	; Push MB info, magic number
	push dword [magic_num]
	push dword [mb_ptr]

	; Call kernel's main function
	call kmain

	; Halt in the event of the kernel returning
	cli
	hlt


section .bss
align 4096
temp_page_dir: resb 4096
temp_page_tab: resb 4096

mb_ptr: resd 1
magic_num: resd 1

stack_space: resb 4096