#ifndef UI_MENU_H
#define UI_MENU_H

#include <stdint.h>
#include "drivers/joystick.h"

void menu_init(void);
void menu_update(JoystickState joystick, uint8_t select_pressed);
void menu_render(void);
uint8_t menu_start_requested(void);

#endif
