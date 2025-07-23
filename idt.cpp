// Interrupt descriptor table

#include "include/idt.h"
#include "include/kernel.h"
#include "include/task.h"

#define IDT_SIZE 256

struct idt_item idt[IDT_SIZE];
struct idt_ptr idt_p;

// Syscall handler
extern "C" void syscall_handler();

extern "C" void page_fault_handler_asm();


// A timer
volatile uint32_t sys_ticks = 0;

// These are external assembly functions that load IDT and handle interrupts
extern "C"
{
    void load_idt(struct idt_ptr* idt_p);
    void kbd_handler(void);
    void isr_stub(void);
    void timer_handler(void);
}

extern "C" void timer_handler_main()
{
	sys_ticks++;
	ob(0x20, 0x20);
    	scheduler();
}

// This handles generic interrupts.
extern "C" void fault_handler()
{
    ob(0x20, 0x20);
}

// Sleep
void sleep_in_ms(uint32_t ms)
{
	// PIT is 100Hz, so 1 tick = 10ms.
	uint32_t wait_time = ms / 10;
	uint32_t wakeup = sys_ticks + wait_time;

	while (sys_ticks < wakeup)
	{
		asm volatile("sti; hlt; cli");
	}
}


void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags)
{
    idt[num].base_lo = (base & 0xFFFF);
    idt[num].base_hi = (base >> 16) & 0xFFFF;
    idt[num].sel = sel;
    idt[num].always0 = 0;
    idt[num].flags = flags;  
}

// Initialize the IDT
void idt_init()
{
    idt_p.lim = (sizeof(struct idt_item) * IDT_SIZE) - 1;
    idt_p.base = (uint32_t)&idt;

    // Set all IDT entries to the generic handler
    for (int i = 0; i < IDT_SIZE; i++)
    {
        idt_set_gate(i, (uint32_t)isr_stub, 0x08, 0x8E);
    }

    idt_set_gate(32, (uint32_t)timer_handler, 0x08, 0x8E);
    idt_set_gate(33, (uint32_t)kbd_handler, 0x08, 0x8E);
    // Syscall interrupt
    idt_set_gate(0x80, (uint32_t)syscall_handler, 0x08, 0xEE);
    idt_set_gate(14, (uint32_t)page_fault_handler_asm, 0x08, 0x8E);


    // Remap PIC
    ob(0x20, 0x11);
    ob(0xA0, 0x11);
    ob(0x21, 0x20);
    ob(0xA1, 0x28);
    ob(0x21, 0x04);
    ob(0xA1, 0x02);
    ob(0x21, 0x01);
    ob(0xA1, 0x01);
    ob(0x21, 0x0);
    ob(0xA1, 0x0);
    ob(0x21, 0xFF);
    ob(0xA1, 0xFF);
    ob(0x21, 0xFD);

    ob(0x21, 0xFC);
    ob(0xA1, 0xFF);


    // Load IDT
    load_idt(&idt_p);
    log(LOG_INFO, "IDT initialized.");
}
