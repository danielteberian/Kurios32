; Handles various interrupts

section.text
extern fault_handler
extern timer_handler_main
extern kbd_handler_main


global isr_stub
global timer_handler
global kbd_handler
global load_idt


isr_stub:
    pusha
    call fault_handler
    popa
    iretd

timer_handler:
    pusha
    call timer_handler_main
    popa
    iretd

kbd_handler:
    pusha
    call kbd_handler_main
    popa
    iretd

load_idt:
    mov eax, [esp + 4]
    lidt [eax]
    ret