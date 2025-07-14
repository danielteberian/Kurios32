global page_fault_handler

section .text
page_fault_handler:
    cli
.hang:
    hlt
    jmp .hang