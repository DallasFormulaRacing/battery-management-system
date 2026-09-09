#include "diagd.h"
#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include <stdint.h>

resource_hogs_t stack_stats;
osThreadId_t ids[20];
volatile size_t g_min_ever_free_heap;

void find_biggest_hog(void) {
  osKernelLock();
  uint32_t count = osThreadGetCount();

  if (osThreadEnumerate(ids, 20) > 0) {
    uint32_t highest_pct = 0;

    for (uint32_t i = 0; i < count; i++) {
      uint32_t total = osThreadGetStackSize(ids[i]);
      uint32_t free_min = osThreadGetStackSpace(ids[i]);
      uint32_t used_max = total - free_min;
      uint32_t pct = (uint32_t)(used_max * 100) / total;

      if (pct > highest_pct) {
        highest_pct = pct;
        stack_stats.thread_handle = ids[i];
        stack_stats.max_bytes_used = used_max;
        stack_stats.percent_used = pct;
      }
    }
  }
  osKernelUnlock();
}

void periodic_get_ever_free_heap_size(void) {
  g_min_ever_free_heap = xPortGetMinimumEverFreeHeapSize();
}
