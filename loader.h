#ifndef LOADER_H
#define LOADER_H

#pragma once
#include <stdint.h>
#include "tasks.h"

int load_program_from_fs(const char *path, pcb_t *out_pcb);

#endif