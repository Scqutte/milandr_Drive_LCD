#ifndef DRIVERS_JOYSTICK_H
#define DRIVERS_JOYSTICK_H

#include <stdint.h>

typedef struct {
    int16_t x;
    int16_t y;
    uint8_t pressed;
} JoystickState;

void joystick_init(void);
JoystickState joystick_read(void);
uint16_t joystick_read_x_raw(void);
uint16_t joystick_read_y_raw(void);

#endif
