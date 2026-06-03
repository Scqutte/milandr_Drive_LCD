#ifndef SYSTEM_TICK_H
#define SYSTEM_TICK_H

#include <stdint.h>

void tick_init(void);
uint32_t tick_get_ms(void);
void tick_delay_ms(uint32_t delay_ms);
uint8_t tick_should_run_frame(void);

#endif
