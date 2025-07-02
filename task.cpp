// Multitasking implementation

#include "task.h"
#include "kernel.h"
#include "mem.h"
#include <stddef.h>

// Currently active task
volatile task_t* active_task;

volatile task_t *ready_tasks;

// Task ID counter
uint32_t next_task_id = 1;

extern "C" void context_switch(reg_t* old_regs, reg_t* new_regs);

// Initialize multitasking
void task_init()
{
    active_task = (task_t*)kmalloc(sizeof(task_t));
    active_task->id = next_task_id++;
    active_task->next = NULL;

    // Kernel is running as soon as the kernel starts, so it is always number 1
    ready_tasks = (task_t*)active_task;
}


// Create task
void task_new(void (*entry_point)())
{
    task_t *new_task = (task_t*)kmalloc(sizeof(task_t));
    uint32_t stack = kmalloc(4096);

    // Set up the new task's registers
    new_task->regs.eip = (uint32_t)entry_point;
    // The stack grows downwards, so point esp to the TOP of the allocated stack area
    new_task->regs.esp = stack + 4096;
    // Interruptaroni are back
    new_task->regs.eflags = 0x202;
    new_task->id = next_task_id++;

    // Add new task to the end of ready tasks list
    task_t *temp_task = (task_t*)ready_tasks;

    while(temp_task->next && temp_task->next != ready_tasks)
    {
        temp_task = temp_task->next;
    }

    temp_task->next = new_task;
    new_task->next = (task_t*)ready_tasks;
}


// Scheduler function
void scheduler()
{
    if (!active_task)
    {
        // No task, no work
        return;
    }

    task_t *prev_task = (task_t*)active_task;
    task_t *next_task = prev_task->next;

    // Don't do anything if there's only one task
    if (!next_task) {
        return;
    }

    active_task = next_task;

    // Switch context
    context_switch(&prev_task->regs, &next_task->regs);
}
