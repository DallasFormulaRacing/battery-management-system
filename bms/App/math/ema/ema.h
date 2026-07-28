#ifndef EMA_H
#define EMA_H

#include <stdint.h>

typedef struct {
    float alpha;
    float current_ema;
    int is_initialized;
} ema_filter_t;

void ema_init(ema_filter_t *filter, float alpha);
float ema_update(ema_filter_t *filter, float new_value);

#endif 