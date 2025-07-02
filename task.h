// Definitions for tasking

#ifndef TASK_H
#define TASK_H

#include <stdint.h>

// Defines the registers that must be saved for each task
typedef struct reg
{
   uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
   uint32_t eip, cs, eflags; 
}
reg_t;

// Task control block (TCB)
typedef struct task
{
    reg_t regs;
    uint32_t id;
    struct task *next;
}
task_t;


// Initialize multitasking
void task_init();

// Create new task
void task_new(void (*entry_point)());

// Scheduler function
void scheduler();

#endif