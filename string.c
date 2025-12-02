// in a freestanding (bare-metal) environment, the standard C library is not
// linked by default. this file provides a few basic string functions used
// throughout the OS, reimplemented from scratch

// implemented functions:
//   - strcmp()  compare two strings
//   - strncmp() compare up to N characters of two strings
//   - strlen()  calculate the length of a string

#include "string.h"

//   the function iterates through both strings until a mismatch is found
//   or one string reaches the null terminator.
//   the difference between the first non-matching characters determines
//   the return value.
int strcmp(const char *a, const char *b) {
    while (*a && (*a == *b)) {
        a++;
        b++;
    }
    return *(const unsigned char *)a - *(const unsigned char *)b;
}

//   similar to strcmp(), but stops early after `n` iterations.
//   useful for checking prefixes or bounded comparisons.
int strncmp(const char *a, const char *b, int n) {
    while (n-- && *a && (*a == *b)) {
        a++;
        b++;
    }
    return n < 0 ? 0 : *(const unsigned char *)a - *(const unsigned char *)b;
}

//   walks through memory from the start of the string until it hits '\0'.
//   does not include the null terminator itself.
int strlen(const char *s) {
    int len = 0;
    while (*s++) len++;
    return len;
}
