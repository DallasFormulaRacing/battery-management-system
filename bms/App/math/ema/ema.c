#include "ema.h"

void ema_init(ema_filter_t *filter, float alpha) {
    filter->alpha = alpha;
    filter->current_ema = 0.0f;
    filter->is_initialized = 0;
}

float ema_update(ema_filter_t *filter, float new_value) {
    
    // intially, 0 will taint the first few values
    if (!filter->is_initialized) {
        filter->current_ema = new_value;
        filter->is_initialized = 1;
    } 
    
    else {
        filter->current_ema = (filter->alpha * new_value) + 
                              ((1.0f - filter->alpha) * filter->current_ema);
    }
    return filter->current_ema;
}