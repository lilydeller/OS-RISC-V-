#include "tasks.h"
#include "uart.h"

/* Very simple spinlock for “synchronization” demo */
static volatile int global_lock = 0;

static task_t tasks[MAX_TASKS];
static int task_count = 0;

static void lock(void) {
    while (__atomic_test_and_set(&global_lock, __ATOMIC_ACQUIRE))
        ; /* spin */
}

static void unlock(void) {
    __atomic_clear(&global_lock, __ATOMIC_RELEASE);
}

/* Example shared counter to show synchronization */
static int shared_counter = 0;

/* Demo tasks */
static void task_counter1(void) {
    lock();
    shared_counter++;
    uart_puts("[task1] counter = ");
    uart_put_dec(shared_counter);
    uart_puts("\n");
    unlock();
}

static void task_counter2(void) {
    lock();
    shared_counter += 2;
    uart_puts("[task2] counter = ");
    uart_put_dec(shared_counter);
    uart_puts("\n");
    unlock();
}

void tasks_init(void) {
    task_count = 0;
    for (int i = 0; i < MAX_TASKS; i++) {
        tasks[i].active = 0;
        tasks[i].id = i;
    }
}

int tasks_add(const char *name, task_step_fn step) {
    if (task_count >= MAX_TASKS) return -1;
    tasks[task_count].name   = name;
    tasks[task_count].step   = step;
    tasks[task_count].active = 1;
    tasks[task_count].id     = task_count;
    return task_count++;
}

void tasks_run_once(void) {
    for (int i = 0; i < task_count; i++) {
        if (tasks[i].active && tasks[i].step) {
            tasks[i].step();
        }
    }
}

void tasks_list(void) {
    uart_puts("ID  Name      Active\n");
    for (int i = 0; i < task_count; i++) {
        uart_put_dec(tasks[i].id);
        uart_puts("   ");
        uart_puts(tasks[i].name);
        uart_puts("   ");
        uart_puts(tasks[i].active ? "yes" : "no");
        uart_puts("\n");
    }
}

void tasks_register_demo_programs(void) {
    tasks_add("counter1", task_counter1);
    tasks_add("counter2", task_counter2);
}
