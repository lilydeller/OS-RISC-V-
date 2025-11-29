#ifndef TASKS_H
#define TASKS_H

#include <stdint.h>

#define MAX_TASKS 8

typedef void (*task_step_fn)(void);

typedef struct {
    int id;                    // Added to match tasks.c
    const char *name;
    task_step_fn step;
    int active;
    int counter;
} task_t;

void tasks_init(void);
void tasks_list(void);
void tasks_run(const char *name);
int  tasks_add(const char *name, task_step_fn step);  // return type matches tasks.c
void tasks_register_demo_programs(void);

#endif
