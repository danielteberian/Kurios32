; Assembly implementation of basic tasking

global context_switch
extern active_task

context_switch:
    mov eax, [esp+4]
    mov ebx, [esp+8]

    pusha
    mov [eax], esp

    mov esp, [ebx]
    popa

    ret