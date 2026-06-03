#include "system/tick.h"

static volatile uint32_t tick_ms;

void tick_init(void)
{
    tick_ms = 0;
}

uint32_t tick_get_ms(void)
{
    return tick_ms;
}

void tick_delay_ms(uint32_t delay_ms)
{
    uint32_t start = tick_get_ms();

    while ((tick_get_ms() - start) < delay_ms) {
    }
}
