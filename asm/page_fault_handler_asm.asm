global page_fault_handler

section .text
page_fault_handler_asm:
    cli
.hang:
    hlt
    jmp .hang