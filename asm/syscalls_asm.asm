global syscall_handler
extern syscall_c_handler

section .text
syscall_handler:
    pusha
    call syscall_c_handler
    popa
    iretd