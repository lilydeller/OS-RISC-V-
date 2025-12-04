#ifndef TASKS_H
#define TASKS_H

#include <stdint.h>

#define MAX_TASKS 8
// Extra variables for tasks.c program loading
#define TASK_MAX_PROC 8
#define TASK_RUNNABLE 1
#define TASK_RUNNING  2
#define TASK_STOPPED  3

typedef void (*task_step_fn)(void);

typedef struct {
    int id;                   
    const char *name;
    task_step_fn step;
    int active;
    int counter;
} task_t;

typedef struct pcb {
    uint32_t pid;
    uint64_t entry;
    uint64_t sp;
    int state;
} pcb_t;

void tasks_init(void);
void tasks_list(void);
void tasks_run(const char *name);
int  tasks_add(const char *name, task_step_fn step);  // return type matches tasks.c
void tasks_create_dynamic_program(const char *name);
void tasks_register_demo_programs(void);
int tasks_alloc_stack(pcb_t *pcb);
void tasks_start_program(pcb_t *pcb);
int tasks_add_pcb(pcb_t *pcb);


#endif
