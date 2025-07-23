; In asm/boot.asm

; Define constants for the multiboot header.
MBALIGN  equ  1<<0            ; Align loaded modules on page boundaries.
MEMINFO  equ  1<<1            ; Provide memory map.
MBFLAGS  equ  MBALIGN | MEMINFO
MAGIC    equ  0x1BADB002      ; Multiboot magic number.
CHECKSUM equ -(MAGIC + MBFLAGS)

; This is the Multiboot header.
section.multiboot
align 4
	dd MAGIC
	dd MBFLAGS
	dd CHECKSUM

; Reserve a stack for the kernel.
section.bss
align 16
stack_bottom:
resb 16384 ; 16 KB
stack_top:

; The linker script specifies _start as the entry point.
section.text
global _start
extern kmain

_start:
    ; The CPU is in 32-bit protected mode when GRUB calls this.
    ; Paging is not yet enabled. The GDT is GRUB's.
    ; We need to set up our own stack and enable paging.

    ; Set up a temporary stack.
    mov esp, stack_top

    ; --- Set up Paging for Higher-Half Kernel ---
    ; We will map the first 4MB of physical memory to the first 4MB of virtual memory (identity map).
    ; We will also map the first 4MB of physical memory to 0xC0000000 (higher half).
    
    ; Clear the page directory and first page table.
    mov edi, page_directory
    mov ecx, 2048 ; 1024 for directory, 1024 for first table
    xor eax, eax
    cld
    rep stosd

    ; Map the first page table into the page directory.
    ; Map it at both 0x00000000 and 0xC0000000.
    mov eax, first_page_table
    or eax, 0x3 ; Present, Read/Write
    mov [page_directory], eax
    mov [page_directory + 768 * 4], eax ; 0xC0000000 >> 22 = 768

    ; Identity map the first 4MB of memory.
    mov edi, first_page_table
    mov ecx, 1024
    mov eax, 0x00000003 ; Start with physical address 0, Present, R/W
  .map_loop:
        stosd
        add eax, 4096 ; Next 4KB page
        loop.map_loop

    ; Load the page directory address into CR3.
    mov eax, page_directory
    mov cr3, eax

    ; Enable paging by setting the PG bit in CR0.
    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax

    ; --- Jump to Higher Half ---
    ; We must do a long jump to a higher-half address to flush the CPU pipeline
    ; and start executing from the new virtual address space.
    lea eax, [higher_half]
    jmp eax

higher_half:
    ; We are now executing in the higher half!
    
    ; Set up the final kernel stack. The linker has already calculated the
    ; correct virtual address for stack_top.
    mov esp, stack_top

    ; Push the multiboot magic number and info structure pointer onto the stack.
    ; These are the arguments for kmain.
    push ebx ; Multiboot info structure
    push eax ; Multiboot magic number

    ; Call the C++ kernel entry point.
    call kmain

    ; If kmain ever returns, hang the system.
    cli
.hang:
    hlt
    jmp.hang

; Page directory and tables must be page-aligned.
section.bss
align 4096
page_directory:
    resd 1024
first_page_table:
    resd 1024