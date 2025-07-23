bits 32
extern page_fault_c_handler

global page_fault_handler_asm

page_fault_handler_asm:
    pusha
    mov eax, [esp + 32]
    push eax
    push esp
    call page_fault_c_handler
    add esp, 8
    cli

.hang:
    hlt
    jmp.hang