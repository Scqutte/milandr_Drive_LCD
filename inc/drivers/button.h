#ifndef DRIVERS_BUTTON_H
#define DRIVERS_BUTTON_H

#include <stdint.h>

#define BUTTON_STATE_FIRE (1U << 0)
#define BUTTON_STATE_UP (1U << 1)
#define BUTTON_STATE_DOWN (1U << 2)

void button_init(void);
void button_update(void);
uint8_t button_state(void);
uint8_t button_fire_pressed(void);
uint8_t button_up_pressed(void);
uint8_t button_down_pressed(void);

#endif
