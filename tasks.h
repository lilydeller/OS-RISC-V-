#ifndef TASKS_H
#define TASKS_H

#include <stdint.h>

typedef void (*task_step_fn)(void);

typedef struct {
    const char   *name;
    task_step_fn step;
    int          active;
    int          id;
} task_t;

#define MAX_TASKS  8

void tasks_init(void);
int  tasks_add(const char *name, task_step_fn step);
void tasks_run_once(void);       /* run one round of all active tasks */
void tasks_list(void);
void tasks_register_demo_programs(void);

#endif
