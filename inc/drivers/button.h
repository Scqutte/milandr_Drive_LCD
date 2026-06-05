#ifndef DRIVERS_BUTTON_H
#define DRIVERS_BUTTON_H

#include <stdint.h>

void button_init(void);
void button_update(void);
uint8_t button_fire_pressed(void);
uint8_t button_up_pressed(void);
uint8_t button_down_pressed(void);

#endif
