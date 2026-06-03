#include "drivers/joystick.h"

void joystick_init(void)
{
}

JoystickState joystick_read(void)
{
    JoystickState state;

    state.x = 0;
    state.y = 0;
    state.pressed = 0;

    return state;
}
