// Switch to user mode
#include <stdint.h>

void enter_umode(uint32_t u_eip, uint32_t u_esp)
{
    asm volatile
    (
        "cli\n"
        "mov $0x23, %%ax\n"
        "mov %%ax, %%ds\n"
        "mov %%ax, %%es\n"
        "mov %%ax, %%fs\n"
        "mov %%ax, %%gs\n"
        "pushl $0x23\n"
        "pushl %[esp]\n"
        "pushf\n"
        "pushl $0x1B\n"
        "pushl %[eip]\n"
        "iret\n"
        :
        : [esp]"r"(u_esp), [eip]"r"(u_eip)
        : "ax"
    );
}