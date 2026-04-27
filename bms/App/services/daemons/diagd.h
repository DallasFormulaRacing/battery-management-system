#ifndef DIAG_D_H
#define DIAG_D_H

#include "cmsis_os2.h"
#include <stdint.h>

typedef struct {
  osThreadId_t thread_handle;
  uint32_t max_bytes_used;
  uint32_t percent_used;
} resource_hogs_t;

extern resource_hogs_t stack_stats;

void find_biggest_hog();

#endif