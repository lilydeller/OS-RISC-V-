#include "uart.h"
#include "tasks.h"

static task_t tasks[MAX_TASKS];
static int task_count = 0;

// task 1: simple counter
static void task_counter1(void) {
    for (int i = 0; i < 5; i++) {
        uart_puts("[counter1] tick ");
        uart_put_dec(i);
        uart_puts("\n");
    }
}

// task 2: another counter
static void task_counter2(void) {
    for (int i = 0; i < 3; i++) {
        uart_puts("[counter2] step ");
        uart_put_dec(i);
        uart_puts("\n");
    }
}

void tasks_init(void) {
    task_count = 0;
    for (int i = 0; i < MAX_TASKS; i++) {
        tasks[i].id = i;
        tasks[i].active = 0;
        tasks[i].counter = 0;
        tasks[i].name = 0;
        tasks[i].step = 0;
    }
}

int tasks_add(const char *name, task_step_fn step) {
    if (task_count >= MAX_TASKS)
        return -1;
    tasks[task_count].id = task_count;
    tasks[task_count].name = name;
    tasks[task_count].step = step;
    tasks[task_count].active = 1;
    return task_count++;
}

void tasks_list(void) {
    uart_puts("Available tasks:\n");
    for (int i = 0; i < task_count; i++) {
        uart_puts("  [");
        uart_put_dec(tasks[i].id);
        uart_puts("] ");
        uart_puts(tasks[i].name);
        uart_puts("\n");
    }
}

void tasks_run(const char *name) {
    for (int i = 0; i < task_count; i++) {
        if (tasks[i].active && tasks[i].name && name) {
            // simple string compare
            const char *a = tasks[i].name, *b = name;
            int match = 1;
            while (*a && *b) {
                if (*a++ != *b++) { match = 0; break; }
            }
            if (match && *a == *b) {
                uart_puts("Running task: ");
                uart_puts(tasks[i].name);
                uart_puts("\n");
                tasks[i].step();
                return;
            }
        }
    }
    uart_puts("No such task.\n");
}

void tasks_register_demo_programs(void) {
    tasks_add("counter1", task_counter1);
    tasks_add("counter2", task_counter2);
}
