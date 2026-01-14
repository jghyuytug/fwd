#include "systems/time_utils.h"

#include <stddef.h>
#include <sys/time.h>

uint32_t get_current_time_ms(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint32_t)((tv.tv_sec * 1000U) + (tv.tv_usec / 1000U));
}
