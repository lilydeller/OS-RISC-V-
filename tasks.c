#include "uart.h"
#include "tasks.h"

//   - tasks_init()
//       initializes the global task array and resets task count.

//   - tasks_add()
//       registers a new task by name and function pointer.

//   - tasks_list()
//       lists all available tasks via UART output.

//   - tasks_run(name)
//       searches for a task by name and executes its function.

//   - tasks_register_demo_programs()
//       registers two built-in demonstration tasks.

// defaults for user base and size
#ifndef USER_BASE
#define USER_BASE 0x80200000UL
#endif

#ifndef USER_SIZE
#define USER_SIZE (32 * 1024 * 1024)
#endif

// also need to keep some sort of constants and allocation for stack resources, especially the top of stack
#define STACK_AREA_BASE (USER_BASE + USER_SIZE)
#define STACK_PER_PROC (64 * 1024)
#define MAX_PROCS       TASK_MAX_PROC

static uint8_t stack_alloc_bitmap[MAX_PROCS]; // 0 = free, 1 = used
static pcb_t pcb_table[MAX_PROCS];
static int next_pid = 1;

// Check --> PCB_T is a proccess control block defined in the .h of this file. it will store information essentially
// and is needed to start the user program in terms of holding the actual binary bitmaps

static task_t tasks[MAX_TASKS];
static int task_count = 0;

// for some reason i need to declare this to prevent an error
extern void start_user(uint64_t entry, uint64_t sp);

// allocate stack memory for the pcb and give it the stack pointer
int tasks_alloc_stack(pcb_t *pcb) {
    for (int i = 0; i < MAX_PROCS; i++) {
        if (!stack_alloc_bitmap[i]) {
            stack_alloc_bitmap[i] = 1;
            uint64_t base = STACK_AREA_BASE + (uint64_t)i * STACK_PER_PROC;
            pcb->sp = base + STACK_PER_PROC; 
            return 0;
        }
    }
    return -1;
}

// simple task that adds pcb to the interal table and gives it a private id
int tasks_add_pcb(pcb_t *pcb) {
    for (int i = 0; i < MAX_PROCS; i++) {
        if (pcb_table[i].pid == 0) {
            pcb->pid = next_pid++;
            pcb_table[i] = *pcb;
            return pcb->pid;
        }
    }
    return -1;
}


// this starts the program using the entry point and the stack pointer. If the user
// returns from start_user, this will priont a message and return the kernel
// ** as opposed to spinning forever (its what userprog.c does)
// we wont reclaim the stack allocation bitmap though, I honestly
// thought it was the cause of the issues I was getting but it was not
// so i never implemented reusing stack slots
void tasks_start_program(pcb_t *pcb) {
    uart_puts(" [TASK] starting the program ... \n");
    start_user(pcb->entry, pcb->sp);
    uart_puts(" [TASK] user program returned to kernel.\n");
    for (int i = 0; i < MAX_PROCS; i++) {
        if (pcb_table[i].pid == pcb->pid) {
            pcb_table[i].pid = 0; 
            break;
        }
    }
}

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

static void task_dynamic_hello(void) {
    uart_puts("[dynamic] Hello from a dynamically created program!\n");
}


// helper used by the shell to create a new program at runtime simply
// adds a task entry that runs task_dynamic_hello() under the given name
// example usage from shell:
//    mkprog hello
//    run hello

void tasks_create_dynamic_program(const char *name) {
    int id = tasks_add(name, task_dynamic_hello);
    if (id < 0) {
        uart_puts("Failed to create program (task table full).\n");
    } else {
        uart_puts("Created program: ");
        uart_puts(name);
        uart_puts("\n");
    }
}

void tasks_register_demo_programs(void) {
    tasks_add("counter1", task_counter1);
    tasks_add("counter2", task_counter2);
}
