#ifndef _INFO_FROM_ENV
#define _INFO_FROM_ENV

#include <unistd.h>
#include <inttypes.h>

int max_cpu_count_env(pid_t pid);

uint64_t mem_limit_env(pid_t pid);

#endif
