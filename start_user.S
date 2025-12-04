.text
.globl start_user
.type start_user, @function
// prototype in C: void start_user(uint64_t entry, uint64_t sp);
start_user:
    // a0 = entry, a1 = sp
    mv sp, a1        // set stack pointer to provided sp
    jr a0            // jump to entry (no return)
