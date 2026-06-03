#ifndef DRIVERS_BUTTON_H
#define DRIVERS_BUTTON_H

#include <stdint.h>

void button_init(void);
uint8_t button_fire_pressed(void);

#endif
